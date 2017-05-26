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

#include "../settings/settings.h"
#include "device.h"

Device::Device(const QString &uuid, bool expires)
    : mUuid(uuid),
      mUsesTls(false),
      mExpires(expires)
{
}

bool Device::hasTimedOut() const
{
    if (mExpires) {
        return QDateTime::currentMSecsSinceEpoch() - mLastPing > Settings::instance()->get(Settings::Key::BroadcastTimeout).toLongLong();
    } else {
        return false;
    }
}

bool Device::update(const QString &name, Platform::OperatingSystem operatingSystem,
                    const QHostAddress &address, quint16 port, bool usesTls)
{
    // Check for any changes
    bool changed = name != mName || operatingSystem != mOperatingSystem || address != mAddress || port != mPort;

    // Store the new values
    mName = name;
    mOperatingSystem = operatingSystem;
    mAddress = address;
    mPort = port;
    mUsesTls = usesTls;

    // Update the last ping
    if (mExpires) {
        mLastPing = QDateTime::currentMSecsSinceEpoch();
    }

    return changed;
}
