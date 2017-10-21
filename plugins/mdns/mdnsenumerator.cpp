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
#include <nitroshare/settings.h>

#include "mdnsenumerator.h"

const QString LoggerTag = "mdns";

const QByteArray ServiceType = "_nitroshare._tcp.local.";

MdnsEnumerator::MdnsEnumerator(Application *application)
    : mLogger(application->logger()),
      mHostname(&mServer),
      mProvider(&mServer, &mHostname),
      mBrowser(&mServer, ServiceType, &mCache)
{
    connect(&mHostname, &QMdnsEngine::Hostname::hostnameChanged, this, &MdnsEnumerator::onHostnameChanged);
    connect(&mBrowser, &QMdnsEngine::Browser::serviceAdded, this, &MdnsEnumerator::onServiceUpdated);
    connect(&mBrowser, &QMdnsEngine::Browser::serviceUpdated, this, &MdnsEnumerator::onServiceUpdated);
    connect(&mBrowser, &QMdnsEngine::Browser::serviceRemoved, this, &MdnsEnumerator::onServiceRemoved);
    connect(application->settings(), &Settings::settingsChanged, this, &MdnsEnumerator::onSettingsChanged);

    // Trigger loading the initial settings
    onSettingsChanged({});
}

void MdnsEnumerator::onHostnameChanged(const QByteArray &hostname) {
    mLogger->log(
        Logger::Debug,
        LoggerTag,
        QString("hostname set to %1").arg(QString(hostname))
    );
}

void MdnsEnumerator::onServiceUpdated(const QMdnsEngine::Service &service)
{
    QString uuid = findUuid(service);
    QVariantMap properties = {
        { DeviceEnumerator::UuidKey, uuid },
        { DeviceEnumerator::NameKey, service.name() },
        { DeviceEnumerator::PortKey, service.port() }
    };
    emit deviceUpdated(uuid, properties);
}

void MdnsEnumerator::onServiceRemoved(const QMdnsEngine::Service &service)
{
    emit deviceRemoved(findUuid(service));
}

void MdnsEnumerator::onSettingsChanged(const QStringList &keys)
{
    QMdnsEngine::Service service;
    service.setType(ServiceType);
    service.setName("");
    service.setPort(0);
    mProvider.update(service);
}

QString MdnsEnumerator::findUuid(const QMdnsEngine::Service &service)
{
    QString uuid = service.attributes().value("uuid");
    if (uuid.isNull()) {
        uuid = service.name();
    }
    return uuid;
}
