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

#ifndef NS_DEVICEMODELPRIVATE_H
#define NS_DEVICEMODELPRIVATE_H

#include <QHostAddress>
#include <QList>
#include <QTimer>

#include "config.h"

#ifdef qmdnsengine_FOUND
#  include <qmdnsengine/browser.h>
#  include <qmdnsengine/cache.h>
#  include <qmdnsengine/server.h>
#endif

#include "../settings/settings.h"
#include "device.h"
#include "devicelistener.h"
#include "devicemodel.h"

class DeviceModelPrivate : public QObject
{
    Q_OBJECT

public:

    explicit DeviceModelPrivate(DeviceModel *deviceModel);
    virtual ~DeviceModelPrivate();

    void update(const QString &uuid, const QString &name, Platform::OperatingSystem operatingSystem,
                const QHostAddress &address, quint16 port, bool usesTls, bool expires);

    DeviceModel * const q;

    QTimer timer;
    DeviceListener listener;

#ifdef qmdnsengine_FOUND
    QMdnsEngine::Server server;
    QMdnsEngine::Cache cache;
    QMdnsEngine::Browser browser;
#endif

    QList<Device*> devices;

private Q_SLOTS:

    void processPing(const QString &uuid, const QString &name, Platform::OperatingSystem operatingSystem,
                     const QHostAddress &address, quint16 port, bool usesTls);
    void cleanup();

    void onSettingsChanged(const QList<Settings::Key> &keys = {});

#ifdef qmdnsengine_FOUND
    void onServiceAddedOrUpdated(const QMdnsEngine::Service &service);
    void onServiceRemoved(const QMdnsEngine::Service &service);
#endif
};

#endif // NS_DEVICEMODELPRIVATE_H
