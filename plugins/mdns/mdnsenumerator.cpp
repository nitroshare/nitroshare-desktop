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

#include <nitroshare/application.h>
#include <nitroshare/logger.h>
#include <nitroshare/message.h>
#include <nitroshare/settingsregistry.h>

#include "mdnsdevice.h"
#include "mdnsenumerator.h"

const QString MessageTag = "mdns";

const QString TransferPort = "TransferPort";

const QByteArray ServiceType = "_nitroshare._tcp.local.";

MdnsEnumerator::MdnsEnumerator(Application *application)
    : mApplication(application),
      mHostname(&mServer),
      mProvider(&mServer, &mHostname),
      mBrowser(&mServer, ServiceType, &mCache)
{
    connect(&mHostname, &QMdnsEngine::Hostname::hostnameChanged, this, &MdnsEnumerator::onHostnameChanged);
    connect(&mBrowser, &QMdnsEngine::Browser::serviceAdded, this, &MdnsEnumerator::onServiceUpdated);
    connect(&mBrowser, &QMdnsEngine::Browser::serviceUpdated, this, &MdnsEnumerator::onServiceUpdated);
    connect(&mBrowser, &QMdnsEngine::Browser::serviceRemoved, this, &MdnsEnumerator::onServiceRemoved);
    connect(application->settingsRegistry(), &SettingsRegistry::settingsChanged, this, &MdnsEnumerator::onSettingsChanged);

    // Initialize the service
    mService.setType(ServiceType);
    mService.setAttributes({{ "uuid", mApplication->deviceUuid().toUtf8() }});

    // Trigger loading the initial settings
    onSettingsChanged({ Application::DeviceName, TransferPort });
}

MdnsEnumerator::~MdnsEnumerator()
{
    qDeleteAll(mDevices);
}

QString MdnsEnumerator::name() const
{
    return "mdns";
}

void MdnsEnumerator::onHostnameChanged(const QByteArray &hostname) {
    mApplication->logger()->log(new Message(
        Message::Info,
        MessageTag,
        QString("hostname set to %1").arg(QString(hostname))
    ));
}

void MdnsEnumerator::onServiceUpdated(const QMdnsEngine::Service &service)
{
    mApplication->logger()->log(new Message(
        Message::Debug,
        MessageTag,
        QString("%1 updated").arg(QString(service.name()))
    ));

    // Attempt to find an existing device, creating one if it does not exist
    MdnsDevice *device = find(service.name());
    bool deviceExisted = static_cast<bool>(device);
    if (!deviceExisted) {
        device = new MdnsDevice(&mServer, &mCache, service);
    }

    // Update the device
    device->update(service);

    // Indicate a new device was added if this is the case
    if (!deviceExisted) {
        mDevices.append(device);
        emit deviceAdded(device);
    }
}

void MdnsEnumerator::onServiceRemoved(const QMdnsEngine::Service &service)
{
    mApplication->logger()->log(new Message(
        Message::Debug,
        MessageTag,
        QString("%1 removed").arg(QString(service.name()))
    ));

    MdnsDevice *device = find(service.name());
    if (device) {
        mDevices.removeOne(device);
        emit deviceRemoved(device);
        delete device;
    }
}

void MdnsEnumerator::onSettingsChanged(const QStringList &keys)
{
    if (keys.contains(Application::DeviceName) || keys.contains(TransferPort)) {
        mService.setName(mApplication->deviceName().toUtf8());
        mService.setPort(mApplication->settingsRegistry()->value(TransferPort).toInt());
        mProvider.update(mService);
    }
}

MdnsDevice *MdnsEnumerator::find(const QString &name) const
{
    for (auto i = mDevices.constBegin(); i != mDevices.constEnd(); ++i) {
        if ((*i)->name() == name) {
            return *i;
        }
    }
    return nullptr;
}
