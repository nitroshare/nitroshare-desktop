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
#include <QSet>

#include "../util/json.h"
#include "devicelistener.h"

DeviceListener::DeviceListener()
{
    connect(&mTimer, &QTimer::timeout, this, &DeviceListener::sendPings);
    connect(&mSocket, &QUdpSocket::readyRead, this, &DeviceListener::processPings);
    connect(Settings::instance(), &Settings::settingsChanged, this, &DeviceListener::onSettingsChanged);

    // Load the initial settings
    onSettingsChanged();

    // Immediately announce the device on the network and start the timer
    sendPings();
    mTimer.start();
}

void DeviceListener::processPings()
{
    while (mSocket.hasPendingDatagrams()) {

        // Capture both the data and the address
        QByteArray data;
        QHostAddress address;

        data.resize(mSocket.pendingDatagramSize());
        mSocket.readDatagram(data.data(), data.size(), &address);

        // Extract the data from the packet
        QJsonDocument document = QJsonDocument::fromJson(data);
        QJsonObject object;
        QString uuid;
        QString name;
        QString operatingSystemName;
        int port;

        if (Json::isObject(document, object) &&
                Json::objectContains(object, "uuid", uuid) &&
                Json::objectContains(object, "name", name) &&
                Json::objectContains(object, "operating_system", operatingSystemName) &&
                Json::objectContains(object, "port", port)) {

            // Double-check that the port is valid
            if (port > 0 && port < 65536) {

                Platform::OperatingSystem operatingSystem = Platform::operatingSystemForName(operatingSystemName);
                bool usesTls = false;
                Json::objectContains(object, "uses_tls", usesTls);

                emit pingReceived(uuid, name, operatingSystem, address, port, usesTls);
            }
        }
    }
}

void DeviceListener::sendPings()
{
    // TODO: when switching to Qt 5.4, switch to using QJsonObject's
    // initializer instead of converting a QVariantMap

    Settings *settings = Settings::instance();

    QJsonObject object(QJsonObject::fromVariantMap({
        { "uuid", settings->get(Settings::Key::DeviceUUID).toString() },
        { "name", settings->get(Settings::Key::DeviceName).toString() },
        { "operating_system", Platform::operatingSystemName() },
        { "port", QString::number(settings->get(Settings::Key::TransferPort).toInt()) },
        { "uses_tls", settings->get(Settings::Key::TLS).toBool() }
    }));

    // Build a list of all unique addresses (since we are
    // sending broadcast packets, these will always be IPv4)
    QSet<QHostAddress> addresses;
    foreach (QNetworkInterface interface, QNetworkInterface::allInterfaces()) {
        if (interface.flags() & QNetworkInterface::CanBroadcast) {
            foreach (QNetworkAddressEntry entry, interface.addressEntries()) {
                if (!entry.broadcast().isNull()) {
                    addresses.insert(entry.broadcast());
                }
            }
        }
    }

    // Build the packet and send it on all of the addresses
    QByteArray data(QJsonDocument(object).toJson(QJsonDocument::Compact));
    foreach (QHostAddress address, addresses) {
        mSocket.writeDatagram(data, address, mSocket.localPort());
    }
}

void DeviceListener::onSettingsChanged(const QList<Settings::Key> &keys)
{
    Settings *settings = Settings::instance();

    if (keys.empty() || keys.contains(Settings::Key::BroadcastInterval)) {
        mTimer.setInterval(settings->get(Settings::Key::BroadcastInterval).toInt());
    }

    if (keys.empty() || keys.contains(Settings::Key::BroadcastPort)) {
        mSocket.close();
        mSocket.bind(settings->get(Settings::Key::BroadcastPort).toInt(), QUdpSocket::ShareAddress);
    }
}
