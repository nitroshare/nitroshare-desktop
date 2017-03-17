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
#  include <sys/socket.h>
#endif

#include <QNetworkAddressEntry>
#include <QNetworkInterface>

#include "dnsutil.h"
#include "mdnsserver.h"

const QHostAddress MdnsIpv4Address("224.0.0.251");
const QHostAddress MdnsIpv6Address("ff02::fb");

const quint16 MdnsPort = 5353;

MdnsServer::MdnsServer()
{
    connect(&mTimer, &QTimer::timeout, this, &MdnsServer::onTimeout);
    connect(&mIpv4Socket, &QUdpSocket::readyRead, this, &MdnsServer::onReadyRead);
    connect(&mIpv6Socket, &QUdpSocket::readyRead, this, &MdnsServer::onReadyRead);

    mTimer.setInterval(60000);
}

bool MdnsServer::start()
{
    if (bindSocket(mIpv4Socket, QHostAddress::AnyIPv4) &&
            bindSocket(mIpv6Socket, QHostAddress::AnyIPv6)) {
        onTimeout();
        mTimer.start();
        return true;
    } else {
        return false;
    }
}

void MdnsServer::stop()
{
    mIpv4Socket.close();
    mIpv6Socket.close();
}

void MdnsServer::sendMessage(const DnsMessage &message)
{
    QByteArray packet;
    DnsUtil::toPacket(message, packet);
    if (message.address().protocol() == QAbstractSocket::IPv4Protocol) {
        mIpv4Socket.writeDatagram(packet, message.address(), message.port());
    }
    if (message.address().protocol() == QAbstractSocket::IPv6Protocol) {
        mIpv6Socket.writeDatagram(packet, message.address(), message.port());
    }
}

QList<QHostAddress> MdnsServer::addressesForHost(const QHostAddress &hostAddress) const
{
    QList<QHostAddress> addresses;
    foreach (QNetworkInterface interface, QNetworkInterface::allInterfaces()) {
        foreach (QNetworkAddressEntry entry, interface.addressEntries()) {
            if (hostAddress.isInSubnet(entry.ip(), entry.prefixLength())) {
                addresses.append(entry.ip());
            }
        }
    }
    return addresses;
}

void MdnsServer::onTimeout()
{
    foreach (QNetworkInterface interface, QNetworkInterface::allInterfaces()) {
        if (interface.flags() & QNetworkInterface::CanMulticast) {
            bool ipv4Address = false;
            bool ipv6Address = false;
            foreach (QHostAddress address, interface.allAddresses()) {
                ipv4Address = ipv4Address || address.protocol() == QAbstractSocket::IPv4Protocol;
                ipv6Address = ipv6Address || address.protocol() == QAbstractSocket::IPv6Protocol;
            }
            if (ipv4Address) {
                mIpv4Socket.joinMulticastGroup(MdnsIpv4Address, interface);
            }
            if (ipv6Address) {
                mIpv6Socket.joinMulticastGroup(MdnsIpv6Address, interface);
            }
        }
    }
}

void MdnsServer::onReadyRead()
{
    QUdpSocket *socket = dynamic_cast<QUdpSocket*>(sender());
    QByteArray packet;
    packet.resize(socket->pendingDatagramSize());
    QHostAddress address;
    quint16 port;
    socket->readDatagram(packet.data(), packet.size(), &address, &port);
    DnsMessage message;
    if (DnsUtil::fromPacket(packet, message)) {
        message.setAddress(address);
        message.setPort(port);
        emit messageReceived(message);
    }
}

bool MdnsServer::bindSocket(QUdpSocket &socket, const QHostAddress &address)
{
    // I cannot find the correct combination of flags that allows the socket
    // to bind properly on Linux, so on that platform, we must manually create
    // the socket and initialize the QUdpSocket with it

#ifdef Q_OS_UNIX
    if (!socket.bind(address, MdnsPort, QAbstractSocket::ShareAddress)) {
        int arg = 1;
        if (setsockopt(socket.socketDescriptor(), SOL_SOCKET, SO_REUSEADDR,
                reinterpret_cast<char*>(&arg), sizeof(int))) {
            return false;
        }
#endif
        return socket.bind(address, MdnsPort, QAbstractSocket::ReuseAddressHint);
#ifdef Q_OS_UNIX
    }
    return true;
#endif
}
