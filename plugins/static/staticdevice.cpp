/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Nathan Osman
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

#include "staticdevice.h"

StaticDevice::StaticDevice(const QString &address)
{
    int index = address.indexOf(':');
    if (index == -1) {
        mAddress = address;
        mPort = 40818;
    } else {
        mAddress = address.left(index);
        mPort = address.mid(index + 1).toInt();
    }
    mName = QString("%1:%2").arg(mAddress).arg(mPort);
}

QString StaticDevice::uuid() const
{
    return mName;
}

QString StaticDevice::name() const
{
    return tr("%1 [static]").arg(mName);
}

QString StaticDevice::transportName() const
{
    return "lan";
}

QStringList StaticDevice::addresses() const
{
    return {mAddress};
}

quint16 StaticDevice::port() const
{
    return mPort;
}
