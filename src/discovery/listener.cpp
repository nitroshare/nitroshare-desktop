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

#include <QJsonDocument>
#include <QNetworkInterface>
#include <QSet>

#include "../util/misc.h"
#include "config.h"
#include "listener.h"

Listener::Listener()
{
    connect(&timer, &QTimer::timeout, this, &Listener::sendPings);
    connect(&socket, &QUdpSocket::readyRead, this, &Listener::processPings);

    initialize();
}

Listener::~Listener()
{
    shutdown();
}

void Listener::start()
{
    sendPings();
    timer.start();
}

void Listener::processPings()
{
    while(socket.hasPendingDatagrams()) {
        QByteArray data;
        data.resize(socket.pendingDatagramSize());
        socket.readDatagram(data.data(), data.size());

        QJsonDocument document(QJsonDocument::fromJson(data));
        if(!document.isEmpty()) {
            emit pingReceived(document.object());
        }
    }
}

void Listener::sendPings()
{
    // TODO: when switching to Qt 5.4, switch to using QJsonObject's
    // initializer instead of converting a QVariantMap

    QJsonObject object(QJsonObject::fromVariantMap({
        { "version", NITROSHARE_VERSION },
        { "uuid", Settings::get<QString>(Settings::DeviceUUID) },
        { "name", Settings::get<QString>(Settings::DeviceName) },
        { "operating_system", currentOperatingSystem() },
        { "port", Settings::get<quint16>(Settings::TransferPort) }
    }));

    QByteArray data(QJsonDocument(object).toJson(QJsonDocument::Compact));
    QSet<QHostAddress> addresses;

    foreach(QNetworkInterface interface, QNetworkInterface::allInterfaces()) {
        if(interface.flags() & QNetworkInterface::CanBroadcast) {
            foreach(QNetworkAddressEntry entry, interface.addressEntries()) {
                if(!entry.broadcast().isNull()) {
                    addresses.insert(entry.broadcast());
                }
            }
        }
    }

    foreach(QHostAddress address, addresses) {
        socket.writeDatagram(data, address, socket.localPort());
    }
}

void Listener::settingChanged(Settings::Key key)
{
    if(key == Settings::BroadcastInterval || key == Settings::BroadcastPort) {
        shutdown();
        initialize();
        start();
    }
}

void Listener::initialize()
{
    timer.setInterval(Settings::get<int>(Settings::BroadcastInterval));
    socket.bind(Settings::get<quint16>(Settings::BroadcastPort), QUdpSocket::ShareAddress);
}

void Listener::shutdown()
{
    timer.stop();
    socket.close();
}
