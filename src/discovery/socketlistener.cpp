/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Nathan Osman
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
 **/

#include <QNetworkInterface>

#include "socketlistener.h"

SocketListener::SocketListener()
{
    connect(&monitor, &InterfaceMonitor::interfaceAdded, this, &SocketListener::addInterface);
    connect(&monitor, &InterfaceMonitor::interfaceRemoved, this, &SocketListener::removeInterface);
    connect(Settings::instance(), &Settings::settingChanged, this, &SocketListener::settingChanged);

    reload();
}

SocketListener::~SocketListener()
{
    QMapIterator<QString, QUdpSocket *> i(sockets);

    while(i.hasNext()) {
        i.value()->deleteLater();
    }
}

void SocketListener::start()
{
    monitor.start();
}

void SocketListener::sendPing()
{
    QMapIterator<QString, QUdpSocket *> i(sockets);

    while(i.hasNext()) {
        i.value()->writeDatagram(Device::current(), multicastAddress, multicastPort);
    }
}

void SocketListener::addInterface(const QString &name)
{
    QUdpSocket * socket = new QUdpSocket;
    QNetworkInterface interface = QNetworkInterface::interfaceFromName(name);

    if(socket->bind(multicastPort, QUdpSocket::ShareAddress) &&
            socket->joinMulticastGroup(multicastAddress, interface)) {
        sockets.insert(name, socket);
        return;
    }

    socket->deleteLater();
}

void SocketListener::removeInterface(const QString &name)
{
    QUdpSocket *socket = sockets.take(name);
    QNetworkInterface interface = QNetworkInterface::interfaceFromName(name);

    socket->leaveMulticastGroup(multicastAddress, interface);
    socket->deleteLater();
}

void SocketListener::processDatagrams()
{
    QUdpSocket * socket = qobject_cast<QUdpSocket *>(sender());

    while(socket->hasPendingDatagrams()) {
        QByteArray data;
        data.resize(socket->pendingDatagramSize());
        socket->readDatagram(data.data(), data.size());

        Device device;
        if(Device::deserialize(data, device)) {
            emit pingReceived(device);
        }
    }
}

void SocketListener::settingChanged(Settings::Key key)
{
    if(key == Settings::MulticastAddress || key == Settings::MulticastPort) {
        reload();
    }
}

void SocketListener::reload()
{
    multicastAddress = QHostAddress(Settings::get<QString>(Settings::MulticastAddress));
    multicastPort = Settings::get<quint16>(Settings::MulticastPort);
}
