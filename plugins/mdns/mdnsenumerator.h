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

#ifndef MDNSENUMERATOR_H
#define MDNSENUMERATOR_H

#include <QMap>

#include <nitroshare/deviceenumerator.h>

#include <qmdnsengine/browser.h>
#include <qmdnsengine/cache.h>
#include <qmdnsengine/hostname.h>
#include <qmdnsengine/provider.h>
#include <qmdnsengine/server.h>
#include <qmdnsengine/service.h>

class Application;

class MdnsDevice;

/**
 * @brief Enumerator using QMdnsEngine to discover peers
 *
 * This enumerator uses the device's name to announce the service on the local
 * network. The LAN plugin provides the transfer port setting and is therefore
 * a dependency of this plugin.
 */
class MdnsEnumerator : public DeviceEnumerator
{
    Q_OBJECT

public:

    explicit MdnsEnumerator(Application *application);
    virtual ~MdnsEnumerator();

private slots:

    void onHostnameChanged(const QByteArray &hostname);
    void onServiceUpdated(const QMdnsEngine::Service &service);
    void onServiceRemoved(const QMdnsEngine::Service &service);
    void onUpdated();
    void onSettingsChanged(const QStringList &keys);

private:

    Application *mApplication;

    QMdnsEngine::Server mServer;
    QMdnsEngine::Hostname mHostname;
    QMdnsEngine::Provider mProvider;
    QMdnsEngine::Cache mCache;
    QMdnsEngine::Browser mBrowser;
    QMdnsEngine::Service mService;

    QMap<QString, MdnsDevice*> mDevices;
};

#endif // MDNSENUMERATOR_H
