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

#include <QTimer>

#include <nitroshare/application.h>
#include <nitroshare/logger.h>
#include <nitroshare/message.h>
#include <nitroshare/settingsregistry.h>

#include <qmdnsengine/resolver.h>

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
    onSettingsChanged({ Application::DeviceName });
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

    // Send an update for the attributes currently known
    QString uuid = findUuid(service);
    QVariantMap properties = {
        { DeviceEnumerator::UuidKey, uuid },
        { DeviceEnumerator::NameKey, service.name() },
        { DeviceEnumerator::PortKey, service.port() }
    };
    emit deviceUpdated(uuid, properties);

    // Send an update every time an address is resolved
    QMdnsEngine::Resolver *resolver = new QMdnsEngine::Resolver(&mServer, service.name(), &mCache, this);
    connect(resolver, &QMdnsEngine::Resolver::resolved, [this, uuid](const QHostAddress &address) {
        emit deviceUpdated(uuid, {{ DeviceEnumerator::AddressesKey, address.toString() }});
    });

    // Delete the resolver after 2 seconds
    QTimer::singleShot(2000, resolver, &QMdnsEngine::Resolver::deleteLater);
}

void MdnsEnumerator::onServiceRemoved(const QMdnsEngine::Service &service)
{
    mApplication->logger()->log(new Message(
        Message::Debug,
        MessageTag,
        QString("%1 removed").arg(QString(service.name()))
    ));

    emit deviceRemoved(findUuid(service));
}

void MdnsEnumerator::onSettingsChanged(const QStringList &keys)
{
    if (keys.contains(Application::DeviceName)) {
        mService.setName(mApplication->deviceName().toUtf8());
        mService.setPort(mApplication->settingsRegistry()->value(TransferPort).toInt());
        mProvider.update(mService);
    }
}

QString MdnsEnumerator::findUuid(const QMdnsEngine::Service &service)
{
    QString uuid = service.attributes().value("uuid");
    if (uuid.isNull()) {
        uuid = service.name();
    }
    return uuid;
}
