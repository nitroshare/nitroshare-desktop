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

#include <QDateTime>

#include "../util/settings.h"
#include "device.h"

Device::Device(const QString &uuid)
    : mUuid(uuid), mPort(0), mLastPing(0)
{
}

void Device::update(const QJsonObject &object, const QHostAddress &address)
{
    if(object.contains("name")) {
        mName = object.value("name").toString();
    } else {
        mName = mUuid;
    }

    if(object.contains("operating_system")) {
        mOperatingSystem = object.value("operating_system").toString();
    } else {
        mOperatingSystem = "unknown";
    }

    mAddress = address;
    mPort = object.value("port").toInt();

    mLastPing = QDateTime::currentMSecsSinceEpoch();
}

bool Device::timeoutReached() const
{
    return QDateTime::currentMSecsSinceEpoch() - mLastPing >=
            Settings::get<qint64>(Settings::BroadcastTimeout);
}
