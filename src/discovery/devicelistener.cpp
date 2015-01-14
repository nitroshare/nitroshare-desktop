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

#include "devicelistener.h"

DeviceListener::DeviceListener()
{
    connect(&timer, &QTimer::timeout, this, &DeviceListener::sendPings);
    connect(&socket, &QUdpSocket::readyRead, this, &DeviceListener::processPings);

    initialize();
}

DeviceListener::~DeviceListener()
{
    shutdown();
}

void DeviceListener::start()
{
    sendPings();
    timer.start();
}

void DeviceListener::processPings()
{
    while(socket.hasPendingDatagrams()) {
        QByteArray data;
        data.resize(socket->pendingDatagramSize());
        socket->readDatagram(data.data(), data.size());

        //...
    }
}

void DeviceListener::sendPings()
{
    QByteArray data = "test";

    foreach(QNetworkInterface interface, QNetworkInterface::allInterfaces()) {
        if(interface.flags() & QNetworkInterface::CanBroadcast) {
            foreach(QNetworkAddressEntry entry, interface.addressEntries()) {
                socket.writeDatagram(data, entry.broadcast(), socket.localPort());
            }
        }
    }
}

void DeviceListener::settingChanged(Settings::Key key)
{
    if(key == Settings::BroadcastInterval || key == Settings::BroadcastPort) {
        shutdown();
        initialize();
        start();
    }
}

void DeviceListener::initialize()
{
    timer.setInterval(Settings::get<int>(Settings::BroadcastInterval));
    socket.bind(Settings::get<quint16>(Settings::BroadcastPort), QUdpSocket::ShareAddress);
}

void DeviceListener::shutdown()
{
    timer.stop();
    socket.close();
}
