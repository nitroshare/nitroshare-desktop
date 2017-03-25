/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Nathan Osman
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include <QHostInfo>
#include <QNetworkAddressEntry>
#include <QNetworkInterface>

#include "mdns.h"
#include "mdnsquery.h"
#include "mdnsservice.h"

const QByteArray BrowsePTR = "_services._dns-sd._udp.local.";

MdnsService::MdnsService(MdnsServer *mdnsServer)
    : mMdnsServer(mdnsServer),
      mPort(0)
{
    connect(mdnsServer, &MdnsServer::messageReceived, this, &MdnsService::onMessageReceived);
}

QByteArray MdnsService::type() const
{
    return mType;
}

void MdnsService::setType(const QByteArray &type)
{
    mType = type;
}

QByteArray MdnsService::name() const
{
    return mName;
}

void MdnsService::setName(const QByteArray &name)
{
    mName = name;
}

QList<QHostAddress> MdnsService::addresses() const
{
    return mAddresses;
}

void MdnsService::setAddresses(const QList<QHostAddress> &addresses)
{
    mAddresses = addresses;
}

quint16 MdnsService::port() const
{
    return mPort;
}

void MdnsService::setPort(quint16 port)
{
    mPort = port;
}

QMap<QByteArray, QByteArray> MdnsService::attributes() const
{
    return mAttributes;
}

void MdnsService::addAttribute(const QByteArray &key, const QByteArray &value)
{
    mAttributes.insert(key, value);
}

void MdnsService::onMessageReceived(const MdnsMessage &message)
{
    if (message.isResponse()) {
        return;
    }
    const QByteArray fqdn = mName + "." + mType;
    bool sendAddresses = false;
    bool sendPtr = false;
    bool sendPtrService = false;
    bool sendSrv = false;
    bool sendTxt = false;
    foreach (MdnsQuery query, message.queries()) {
        switch (query.type()) {
        case Mdns::A:
        case Mdns::AAAA:
            sendAddresses = true;
            break;
        case Mdns::PTR:
            if (query.name() == mType) {
                sendAddresses = true;
                sendPtr = true;
                sendSrv = true;
                sendTxt = true;
            } else if (query.name() == BrowsePTR) {
                sendPtrService = true;
            }
            break;
        case Mdns::SRV:
            if (query.name() == fqdn) {
                sendAddresses = true;
                sendSrv = true;
            }
            break;
        case Mdns::TXT:
            if (query.name() == fqdn) {
                sendTxt = true;
            }
            break;
        default:
            break;
        }
    }
    if (!sendAddresses && !sendPtr && !sendPtrService && !sendSrv && !sendTxt) {
        return;
    }
    QHostAddress address = findInterfaceAddress(message.address());
    if (address.isNull()) {
        return;
    }
    MdnsMessage reply;
    reply.setTransactionId(message.transactionId());
    reply.setResponse(true);
    reply.setPort(message.port());
    reply.setAddress(message.port() == 5353 ?
        (address.protocol() == QAbstractSocket::IPv4Protocol ?
             Mdns::Ipv4Address :
             Mdns::Ipv6Address) :
        message.address());
    if (sendAddresses) {
        reply.addRecord(generateAddress(address));
    }
    if (sendPtr) {
        reply.addRecord(generatePtr());
    }
    if (sendPtrService) {
        reply.addRecord(generatePtrService());
    }
    if (sendSrv) {
        reply.addRecord(generateSrv());
    }
    if (sendTxt) {
        reply.addRecord(generateTxt());
    }
    mMdnsServer->sendMessage(reply);
}

QHostAddress MdnsService::findInterfaceAddress(const QHostAddress &hostAddress)
{
    foreach (QNetworkInterface interface, QNetworkInterface::allInterfaces()) {
        foreach (QNetworkAddressEntry entry, interface.addressEntries()) {
            if (hostAddress.isInSubnet(entry.ip(), entry.prefixLength())) {
                return entry.ip();
            }
        }
    }
    return QHostAddress();
}

MdnsRecord MdnsService::generateAddress(const QHostAddress &address)
{
    MdnsRecord record;
    record.setName(QString(QHostInfo::localHostName() + ".local.").toUtf8());
    record.setType(address.protocol() == QAbstractSocket::IPv4Protocol ? Mdns::A : Mdns::AAAA);
    record.setTtl(3600);
    record.setAddress(address);
    return record;
}

MdnsRecord MdnsService::generatePtrService()
{
    MdnsRecord record;
    record.setName(BrowsePTR);
    record.setType(Mdns::PTR);
    record.setTtl(3600);
    record.setTarget(mType);
    return record;
}

MdnsRecord MdnsService::generatePtr()
{
    MdnsRecord record;
    record.setName(mType);
    record.setType(Mdns::PTR);
    record.setTtl(3600);
    record.setTarget(mName + "." + mType);
    return record;
}

MdnsRecord MdnsService::generateSrv()
{
    MdnsRecord record;
    record.setName(mName + "." + mType);
    record.setType(Mdns::SRV);
    record.setPort(mPort);
    record.setTtl(3600);
    record.setTarget(QString(QHostInfo::localHostName() + ".local.").toUtf8());
    return record;
}

MdnsRecord MdnsService::generateTxt()
{
    MdnsRecord record;
    record.setName(mName + "." + mType);
    record.setType(Mdns::TXT);
    record.setTtl(3600);
    record.setTxt(mAttributes);
    return record;
}
