/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Nathan Osman
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

#include <QDateTime>
#include <QHostAddress>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAddressEntry>
#include <QNetworkInterface>
#include <QSet>

#include <nitroshare/application.h>
#include <nitroshare/category.h>
#include <nitroshare/logger.h>
#include <nitroshare/message.h>
#include <nitroshare/settingsregistry.h>

#include "broadcastdevice.h"
#include "broadcastenumerator.h"

const QString MessageTag = "broadcast";

const QString BroadcastCategory = "broadcast";
const QString BroadcastInterval = "BroadcastInterval";
const QString BroadcastExpiry = "BroadcastExpiry";
const QString BroadcastPort = "BroadcastPort";

const QString TransferPort = "TransferPort";

BroadcastEnumerator::BroadcastEnumerator(Application *application)
    : mApplication(application),
      mBroadcastCategory({
          { Category::NameKey, BroadcastCategory },
          { Category::TitleKey, tr("Broadcast") },
      }),
      mBroadcastInterval({
          { Setting::TypeKey, Setting::Integer },
          { Setting::NameKey, BroadcastInterval },
          { Setting::TitleKey, tr("Broadcast Interval") },
          { Setting::CategoryKey, BroadcastCategory },
          { Setting::DefaultValueKey, 5000 }
      }),
      mBroadcastExpiry({
          { Setting::TypeKey, Setting::Integer },
          { Setting::NameKey, BroadcastExpiry },
          { Setting::TitleKey, tr("Broadcast Expiry") },
          { Setting::CategoryKey, BroadcastCategory },
          { Setting::DefaultValueKey, 30000 }
      }),
      mBroadcastPort({
          { Setting::TypeKey, Setting::Integer },
          { Setting::NameKey, BroadcastPort },
          { Setting::TitleKey, tr("Broadcast Port") },
          { Setting::CategoryKey, BroadcastCategory },
          { Setting::DefaultValueKey, 40816 }
      })
{
    connect(&mBroadcastTimer, &QTimer::timeout, this, &BroadcastEnumerator::onBroadcastTimeout);
    connect(&mExpiryTimer, &QTimer::timeout, this, &BroadcastEnumerator::onExpiryTimeout);
    connect(&mSocket, &QUdpSocket::readyRead, this, &BroadcastEnumerator::onReadyRead);
    connect(mApplication->settingsRegistry(), &SettingsRegistry::settingsChanged, this, &BroadcastEnumerator::onSettingsChanged);

    mApplication->settingsRegistry()->addCategory(&mBroadcastCategory);
    mApplication->settingsRegistry()->addSetting(&mBroadcastInterval);
    mApplication->settingsRegistry()->addSetting(&mBroadcastExpiry);
    mApplication->settingsRegistry()->addSetting(&mBroadcastPort);

    // Trigger loading the initial settings
    onSettingsChanged({ BroadcastInterval, BroadcastExpiry, BroadcastPort });
}

BroadcastEnumerator::~BroadcastEnumerator()
{
    mApplication->settingsRegistry()->removeSetting(&mBroadcastInterval);
    mApplication->settingsRegistry()->removeSetting(&mBroadcastExpiry);
    mApplication->settingsRegistry()->removeSetting(&mBroadcastPort);
    mApplication->settingsRegistry()->removeCategory(&mBroadcastCategory);

    qDeleteAll(mDevices);
}

QString BroadcastEnumerator::name() const
{
    return "broadcast";
}

void BroadcastEnumerator::onBroadcastTimeout()
{
    // Build a list of all IPv4 broadcast addresses
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

    // Build the packet that will be broadcast
    QJsonObject object{
        { "uuid", mApplication->deviceUuid() },
        { "name", mApplication->deviceName() },
        { "port", mApplication->settingsRegistry()->value(TransferPort).toInt() }
    };
    QByteArray data = QJsonDocument(object).toJson(QJsonDocument::Compact);

    // Broadcast the packet
    foreach (QHostAddress address, addresses) {
        mSocket.writeDatagram(data, address, mSocket.localPort());
    }
}

void BroadcastEnumerator::onExpiryTimeout()
{
    qint64 curMs = QDateTime::currentMSecsSinceEpoch();
    int timeoutMs = mApplication->settingsRegistry()->value(BroadcastExpiry).toInt();

    // Remove any devices that have expired
    for (auto i = mDevices.begin(); i != mDevices.end();) {
        if ((*i)->isExpired(curMs, timeoutMs)) {
            emit deviceRemoved(*i);
            mDevices.erase(i);
            delete *i;
        } else {
            ++i;
        }
    }
}

void BroadcastEnumerator::onReadyRead()
{
    qint64 curMs = QDateTime::currentMSecsSinceEpoch();

    // Read all of the packets
    while (mSocket.hasPendingDatagrams()) {

        // Capture both the data and the address
        QByteArray data;
        QHostAddress address;

        // Receive the packet
        data.resize(mSocket.pendingDatagramSize());
        mSocket.readDatagram(data.data(), data.size(), &address);
        QJsonObject object = QJsonDocument::fromJson(data).object();

        // Ensure the packet includes UUID
        QString uuid = object.value("uuid").toString();
        if (uuid.isNull()) {
            continue;
        }

        // Attempt to find an existing device that matches
        BroadcastDevice *device = nullptr;
        for (auto i = mDevices.constBegin(); i != mDevices.constEnd(); ++i) {
            if ((*i)->uuid() == uuid) {
                device = *i;
            }
        }

        // If no existing device was found, emit a new device
        bool deviceExisted = static_cast<bool>(device);
        if (!deviceExisted) {
            device = new BroadcastDevice;
        }

        // Update the device
        device->update(curMs, address, object);

        // Indicate that a new device was discovered
        if (!deviceExisted) {
            mDevices.append(device);
            emit deviceAdded(device);
        }
    }
}

void BroadcastEnumerator::onSettingsChanged(const QStringList &keys)
{
    if (keys.contains(BroadcastInterval)) {
        mBroadcastTimer.stop();
        mBroadcastTimer.setInterval(
            mApplication->settingsRegistry()->value(BroadcastInterval).toInt()
        );
        onBroadcastTimeout();
        mBroadcastTimer.start();
    }

    if (keys.contains(BroadcastExpiry)) {
        mExpiryTimer.stop();
        mExpiryTimer.setInterval(
            mApplication->settingsRegistry()->value(BroadcastExpiry).toInt()
        );
        onExpiryTimeout();
        mExpiryTimer.start();
    }

    if (keys.contains(BroadcastPort)) {
        mSocket.close();
        if (!mSocket.bind(QHostAddress::AnyIPv4,
                mApplication->settingsRegistry()->value(BroadcastPort).toInt())) {
            mApplication->logger()->log(new Message(
                Message::Error,
                MessageTag,
                mSocket.errorString()
            ));
        }
    }
}
