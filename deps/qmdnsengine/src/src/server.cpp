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

#ifdef Q_OS_UNIX
#  include <cerrno>
#  include <cstring>
#  include <sys/socket.h>
#endif

#include <QHostAddress>
#include <QNetworkInterface>

#include <qmdnsengine/dns.h>
#include <qmdnsengine/mdns.h>
#include <qmdnsengine/message.h>
#include <qmdnsengine/server.h>

#include "server_p.h"

using namespace QMdnsEngine;

ServerPrivate::ServerPrivate(Server *server)
    : QObject(server),
      q(server)
{
    connect(&timer, &QTimer::timeout, this, &ServerPrivate::onTimeout);
    connect(&ipv4Socket, &QUdpSocket::readyRead, this, &ServerPrivate::onReadyRead);
    connect(&ipv6Socket, &QUdpSocket::readyRead, this, &ServerPrivate::onReadyRead);

    timer.setInterval(60 * 1000);
    timer.setSingleShot(true);
    onTimeout();
}

void ServerPrivate::bindSocket(QUdpSocket &socket, const QHostAddress &address)
{
    // I cannot find the correct combination of flags that allows the socket
    // to bind properly on Linux, so on that platform, we must manually create
    // the socket and initialize the QUdpSocket with it

#ifdef Q_OS_UNIX
    if (!socket.bind(address, MdnsPort, QAbstractSocket::ShareAddress)) {
        int arg = 1;
        if (setsockopt(socket.socketDescriptor(), SOL_SOCKET, SO_REUSEADDR,
                reinterpret_cast<char*>(&arg), sizeof(int))) {
            emit q->error(strerror(errno));
        }
#endif
        if (!socket.bind(address, MdnsPort, QAbstractSocket::ReuseAddressHint)) {
            emit q->error(socket.errorString());
        }
#ifdef Q_OS_UNIX
    }
#endif
}

void ServerPrivate::onTimeout()
{
    // Bind the sockets if not already bound
    if (ipv4Socket.state() != QAbstractSocket::BoundState) {
        bindSocket(ipv4Socket, QHostAddress::AnyIPv4);
    }
    if (ipv6Socket.state() != QAbstractSocket::BoundState) {
        bindSocket(ipv6Socket, QHostAddress::AnyIPv6);
    }
    bool ipv4Bound = ipv4Socket.state() == QAbstractSocket::BoundState;
    bool ipv6Bound = ipv6Socket.state() == QAbstractSocket::BoundState;

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
                if (ipv4Bound && ipv4Address) {
                    ipv4Socket.joinMulticastGroup(MdnsIpv4Address, interface);
                }
                if (ipv6Bound && ipv6Address) {
                    ipv6Socket.joinMulticastGroup(MdnsIpv6Address, interface);
                }
            }
        }
    }

    // Schedule the timeout in one minute
    timer.start();
}

void ServerPrivate::onReadyRead()
{
    // Read the packet from the socket
    QUdpSocket *socket = dynamic_cast<QUdpSocket*>(sender());
    QByteArray packet;
    packet.resize(socket->pendingDatagramSize());
    QHostAddress address;
    quint16 port;
    socket->readDatagram(packet.data(), packet.size(), &address, &port);

    // Attempt to decode the packet
    Message message;
    if (fromPacket(packet, message)) {
        message.setAddress(address);
        message.setPort(port);
        emit q->messageReceived(message);
    }
}

Server::Server(QObject *parent)
    : AbstractServer(parent),
      d(new ServerPrivate(this))
{
}

void Server::sendMessage(const Message &message)
{
    QByteArray packet;
    toPacket(message, packet);
    if (message.address().protocol() == QAbstractSocket::IPv4Protocol) {
        d->ipv4Socket.writeDatagram(packet, message.address(), message.port());
    } else {
        d->ipv6Socket.writeDatagram(packet, message.address(), message.port());
    }
}

void Server::sendMessageToAll(const Message &message)
{
    QByteArray packet;
    toPacket(message, packet);
    d->ipv4Socket.writeDatagram(packet, MdnsIpv4Address, MdnsPort);
    d->ipv6Socket.writeDatagram(packet, MdnsIpv6Address, MdnsPort);
}
