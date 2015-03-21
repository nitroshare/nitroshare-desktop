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

#ifndef NS_DEVICE_H
#define NS_DEVICE_H

#include <QHostAddress>

#include "../util/platform.h"

/**
 * @brief Information about another device on the network
 *
 * The members of this class are populated by the DeviceModel class when pings
 * are received. The information needed to transfer files to the device are
 * contained here.
 */
class Device
{
public:

    explicit Device(const QString &uuid);

    QString uuid() const { return mUuid; }
    QString name() const { return mName; }
    Platform::OperatingSystem operatingSystem() const { return mOperatingSystem; }
    QHostAddress address() const { return mAddress; }
    quint16 port() const { return mPort; }

    bool hasTimedOut() const;
    bool update(const QString &name, Platform::OperatingSystem operatingSystem, const QHostAddress &address, quint16 port);

private:

    const QString mUuid;

    QString mName;
    Platform::OperatingSystem mOperatingSystem;
    QHostAddress mAddress;
    quint16 mPort;

    qint64 mLastPing = 0;
};

#endif // NS_DEVICE_H
