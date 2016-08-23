/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Nathan Osman
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

#ifndef BROADCASTENUMERATOR_H
#define BROADCASTENUMERATOR_H

#include <QMap>
#include <QTimer>
#include <QUdpSocket>

#include <nitroshare/deviceenumerator.h>

class Application;

/**
 * @brief Discover devices through IPv4 broadcast
 *
 * This plugin allows for interoperability with the 0.3.x releases which use
 * IPv4 broadcast for device discovery. The mDNS plugin will eventually
 * replace this plugin due to its support for IPv6 and less noise.
 *
 * If no ping is received from a device after a fixed amount of time has
 * expired, the device is considered "expired" and is removed.
 */
class BroadcastEnumerator : public DeviceEnumerator
{
    Q_OBJECT

public:

    explicit BroadcastEnumerator(Application *application);

private slots:

    void onBroadcastTimeout();
    void onExpiryTimeout();
    void onReadyRead();
    void onSettingsChanged(const QStringList &keys);

private:

    Application *mApplication;

    QTimer mBroadcastTimer;
    QTimer mExpiryTimer;
    QUdpSocket mSocket;
    QMap<QString, qlonglong> mDevices;
};

#endif // BROADCASTENUMERATOR_H
