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

#include <QtGlobal>

#ifdef Q_OS_LINUX
#  include <cerrno>
#  include <cstring>
#  include <sys/socket.h>
#endif

#include <QHostInfo>
#include <QNetworkAddressEntry>
#include <QNetworkInterface>

#include "mdns.h"
#include "mdnsquery.h"
#include "mdnsrecord.h"
#include "mdnsserver.h"

// TODO: watch for the sockets disconnecting and retry hostname registration

MdnsServer::MdnsServer()
    : mHostnameConfirmed(false)
{
    connect(&mSocketTimer, &QTimer::timeout, this, &MdnsServer::onSocketTimeout);
    connect(&mHostnameTimer, &QTimer::timeout, this, &MdnsServer::onHostnameTimeout);
    connect(&mIpv4Socket, &QUdpSocket::readyRead, this, &MdnsServer::onReadyRead);
    connect(&mIpv6Socket, &QUdpSocket::readyRead, this, &MdnsServer::onReadyRead);
    connect(this, &MdnsServer::messageReceived, this, &MdnsServer::onMessageReceived);

    // Prepare the timers
    mSocketTimer.setSingleShot(true);
    mHostnameTimer.setSingleShot(true);

    // Start joining the multicast addresses
    onSocketTimeout();
}

void MdnsServer::sendMessage(const MdnsMessage &message)
{
    QByteArray packet;
    Mdns::toPacket(message, packet);
    if (message.protocol() == Mdns::Protocol::IPv4) {
        mIpv4Socket.writeDatagram(packet, message.address(), message.port());
    }
    if (message.protocol() == Mdns::Protocol::IPv6) {
        mIpv6Socket.writeDatagram(packet, message.address(), message.port());
    }
}

QHostAddress MdnsServer::address(const QHostAddress &address) const
{
    foreach (QNetworkInterface interface, QNetworkInterface::allInterfaces()) {
        foreach (QNetworkAddressEntry entry, interface.addressEntries()) {
            if (address.isInSubnet(entry.ip(), entry.prefixLength())) {
                return entry.ip();
            }
        }
    }
    return QHostAddress();
}

void MdnsServer::onSocketTimeout()
{
    // Bind the sockets if not already bound
    if (mIpv4Socket.state() != QAbstractSocket::BoundState) {
        bindSocket(mIpv4Socket, QHostAddress::AnyIPv4);
    }
    if (mIpv6Socket.state() != QAbstractSocket::BoundState) {
        bindSocket(mIpv6Socket, QHostAddress::AnyIPv6);
    }
    bool ipv4Bound = mIpv4Socket.state() == QAbstractSocket::BoundState;
    bool ipv6Bound = mIpv6Socket.state() == QAbstractSocket::BoundState;

    // Assuming either of the sockets are bound, join multicast groups
    if (ipv4Bound || ipv6Bound) {
        foreach (QNetworkInterface interface, QNetworkInterface::allInterfaces()) {
            if (interface.flags() & QNetworkInterface::CanMulticast) {
                bool ipv4Address = false;
                bool ipv6Address = false;
                foreach (QHostAddress address, interface.allAddresses()) {
                    ipv4Address = ipv4Address || address.protocol() == QAbstractSocket::IPv4Protocol;
                    ipv6Address = ipv6Address || address.protocol() == QAbstractSocket::IPv6Protocol;
                }
                if (ipv4Bound && ipv6Address) {
                    mIpv4Socket.joinMulticastGroup(Mdns::Ipv4Address, interface);
                }
                if (ipv6Bound && ipv6Address) {
                    mIpv6Socket.joinMulticastGroup(Mdns::Ipv6Address, interface);
                }
            }
        }

        // If the hostname has not been set, begin checking hostnames
        if (!mHostnameConfirmed) {
            mHostname = QHostInfo::localHostName() + ".local.";
            mHostnameSuffix = 1;
            checkHostname(Mdns::Protocol::IPv4);
            checkHostname(Mdns::Protocol::IPv6);
        }
    }

    // Run the method again in one minute
    mSocketTimer.start(60 * 1000);
}

void MdnsServer::onHostnameTimeout()
{
    // There was no response for the hostname query, so it can be used
    mHostnameConfirmed = true;
    emit hostnameConfirmed(mHostname);
}

void MdnsServer::onReadyRead()
{
    QUdpSocket *socket = dynamic_cast<QUdpSocket*>(sender());
    QByteArray packet;
    packet.resize(socket->pendingDatagramSize());
    QHostAddress address;
    quint16 port;
    socket->readDatagram(packet.data(), packet.size(), &address, &port);
    MdnsMessage message;
    if (Mdns::fromPacket(packet, message)) {
        message.setAddress(address);
        message.setProtocol(address.protocol() == QAbstractSocket::IPv4Protocol ?
            Mdns::Protocol::IPv4 : Mdns::Protocol::IPv6);
        message.setPort(port);
        emit messageReceived(message);
    }
}

void MdnsServer::onMessageReceived(const MdnsMessage &message)
{
    if (mHostnameConfirmed) {
        if (!message.isResponse()) {
            bool queryA = false;
            bool queryAAAA = false;
            foreach (MdnsQuery query, message.queries()) {
                if (query.name() == mHostname) {
                    queryA = queryA || query.type() == Mdns::A;
                    queryAAAA = queryAAAA || query.type() == Mdns::AAAA;
                }
            }
            if (queryA || queryAAAA) {
                MdnsMessage reply = message.reply();
                //queryA && reply.addRecord(generateRecord(Mdns::A));
                //queryAAAA && reply.addRecord(generateRecord(Mdns::AAAA));
                sendMessage(reply);
            }
        }
    } else {
        if (message.isResponse()) {
            foreach (MdnsRecord record, message.records()) {
                if ((record.type() == Mdns::A || record.type() == Mdns::AAAA) &&
                        record.name() == mHostname && record.ttl()) {
                    QString suffix = QString("-%1").arg(mHostnameSuffix++);
                    mHostname = QString("%1%2.local.").arg(QHostInfo::localHostName()).arg(suffix);
                    checkHostname(Mdns::Protocol::IPv4);
                    checkHostname(Mdns::Protocol::IPv6);
                    break;
                }
            }
        }
    }
}

void MdnsServer::bindSocket(QUdpSocket &socket, const QHostAddress &address)
{
    // I cannot find the correct combination of flags that allows the socket
    // to bind properly on Linux, so on that platform, we must manually create
    // the socket and initialize the QUdpSocket with it

#ifdef Q_OS_UNIX
    if (!socket.bind(address, Mdns::Port, QAbstractSocket::ShareAddress)) {
        int arg = 1;
        if (setsockopt(socket.socketDescriptor(), SOL_SOCKET, SO_REUSEADDR,
                reinterpret_cast<char*>(&arg), sizeof(int))) {
            emit error(strerror(errno));
        }
#endif
        if (!socket.bind(address, Mdns::Port, QAbstractSocket::ReuseAddressHint)) {
            emit error(socket.errorString());
        }
#ifdef Q_OS_UNIX
    }
#endif
}

void MdnsServer::checkHostname(Mdns::Protocol protocol)
{
    MdnsQuery query;
    query.setName(mHostname.toUtf8());
    query.setType(Mdns::A);

    MdnsMessage message;
    message.setAddress(protocol == Mdns::Protocol::IPv4 ?
        Mdns::Ipv4Address : Mdns::Ipv6Address);
    message.setProtocol(protocol);
    message.setPort(Mdns::Port);
    message.addQuery(query);

    sendMessage(message);
}
