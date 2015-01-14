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

#include <QMapIterator>
#include <QMutableMapIterator>
#include <QNetworkInterface>

#include "socketlistener.h"

SocketListener::SocketListener()
{
    connect(&monitor, &InterfaceMonitor::interfaceAdded, this, &SocketListener::addInterface);
    connect(&monitor, &InterfaceMonitor::interfaceRemoved, this, &SocketListener::removeInterface);
    connect(&timer, &QTimer::timeout, this, &SocketListener::sendPing);
    connect(Settings::instance(), &Settings::settingChanged, this, &SocketListener::settingChanged);

    reload();
}

SocketListener::~SocketListener()
{
    QMapIterator<QString, QUdpSocket *> i(sockets);

    while(i.hasNext()) {
        shutdown(i.next().value(), true);
    }
}

void SocketListener::start()
{
    monitor.start();
    timer.start();
}

void SocketListener::addInterface(const QString &name)
{
    QUdpSocket * socket = new QUdpSocket;

    if(initialize(socket, name)) {
        sockets.insert(name, socket);
        return;
    }

    socket->deleteLater();
}

void SocketListener::removeInterface(const QString &name)
{
    shutdown(sockets.take(name), true);
}

void SocketListener::sendPing()
{
    QMapIterator<QString, QUdpSocket *> i(sockets);

    while(i.hasNext()) {
        i.next().value()->writeDatagram(Device::current(), multicastAddress, multicastPort);
    }
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
    if(key == Settings::PingInterval || key == Settings::MulticastAddress ||
            key == Settings::MulticastPort) {
        {
            QMapIterator<QString, QUdpSocket *> i(sockets);

            while(i.hasNext()) {
                shutdown(i.next().value(), false);
            }
        }

        timer.stop();
        reload();
        timer.start();

        {
            QMutableMapIterator<QString, QUdpSocket *> i(sockets);

            while(i.hasNext()) {
                i.next();
                if(!initialize(i.value(), i.key())) {
                    shutdown(i.value(), true);
                    i.remove();
                }
            }
        }
    }
}

void SocketListener::reload()
{
    timer.setInterval(Settings::get<int>(Settings::PingInterval));

    multicastAddress = QHostAddress(Settings::get<QString>(Settings::MulticastAddress));
    multicastPort = Settings::get<quint16>(Settings::MulticastPort);
}

bool SocketListener::initialize(QUdpSocket *socket, const QString &name)
{
    QNetworkInterface interface = QNetworkInterface::interfaceFromName(name);

    return socket->bind(multicastPort, QUdpSocket::ShareAddress) &&
            socket->joinMulticastGroup(multicastAddress, interface);
}

void SocketListener::shutdown(QUdpSocket *socket, bool destroy)
{
    socket->leaveMulticastGroup(multicastAddress, socket->multicastInterface());
    socket->close();

    if(destroy) {
        socket->deleteLater();
    }
}
