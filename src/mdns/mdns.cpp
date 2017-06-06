/**
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
 **/

#include <QVariant>

#include "mdns.h"

#include <qmdnsengine/service.h>

Mdns::Mdns()
    : mServer(nullptr),
      mHostname(nullptr),
      mProvider(nullptr)
{
    connect(Settings::instance(), &Settings::settingsChanged, this, &Mdns::onSettingsChanged);

    // Ensure the service is provided as soon as possible
    onSettingsChanged(QList<Settings::Key>());
}

void Mdns::onHostnameChanged(const QByteArray &hostname)
{
    qDebug("Hostname changed: %s", hostname.constData());
}

void Mdns::onSettingsChanged(const QList<Settings::Key> &keys)
{
    Settings *settings = Settings::instance();

    if (keys.empty() || keys.contains(Settings::Key::BehaviorReceive) ||
            keys.contains(Settings::Key::TransferPort) ||
            keys.contains(Settings::Key::DeviceUUID) ||
            keys.contains(Settings::Key::DeviceName)) {
        if (settings->get(Settings::Key::BehaviorReceive).toBool()) {
            if (!mProvider) {
                mServer = new QMdnsEngine::Server(this);
                mHostname = new QMdnsEngine::Hostname(mServer, mServer);
                mProvider = new QMdnsEngine::Provider(mServer, mHostname, mServer);
                connect(mHostname, &QMdnsEngine::Hostname::hostnameChanged, this, &Mdns::onHostnameChanged);
            }
            QMdnsEngine::Service service;
            service.setName(settings->get(Settings::Key::DeviceName).toString().toUtf8());
            service.setType("_nitroshare._tcp.local.");
            service.setPort(Settings::instance()->get(Settings::Key::TransferPort).toInt());
            service.setAttributes({{"uuid", settings->get(Settings::Key::DeviceUUID).toString().toUtf8()}});
            mProvider->update(service);
        } else {
            if (mProvider) {
                delete mProvider;
                delete mHostname;
                delete mServer;
                mProvider = nullptr;
                mHostname = nullptr;
                mServer = nullptr;
            }
        }
    }
}
