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

#include <QMetaObject>

#include "../socket/socketreader.h"
#include "../socket/socketwriter.h"
#include "transfer.h"

Transfer::Transfer(qintptr socketDescriptor)
    : mConnection(new SocketReader(socketDescriptor)),
      mDeviceName(tr("[unknown]")), mProgress(0), mDirection(Receive)
{
    initialize();
}

Transfer::Transfer(DevicePointer device, BundlePointer bundle)
    : mConnection(new SocketWriter(device->address(), device->port(), bundle)),
      mDeviceName(device->name()), mProgress(0), mDirection(Send)
{
    initialize();
}

Transfer::~Transfer()
{
    mThread.quit();
    mThread.wait();
}

QString Transfer::deviceName() const
{
    return mDeviceName;
}

int Transfer::progress() const
{
    return mProgress;
}

Transfer::Direction Transfer::direction() const
{
    return mDirection;
}

void Transfer::start()
{
    QMetaObject::invokeMethod(mConnection.data(), "start", Qt::QueuedConnection);
}

void Transfer::cancel()
{
    QMetaObject::invokeMethod(mConnection.data(), "cancel", Qt::QueuedConnection);
}

void Transfer::setDeviceName(const QString &deviceName)
{
    mDeviceName = deviceName;
}

void Transfer::setProgress(int progress)
{
    mProgress = progress;
}

void Transfer::initialize()
{
    mThread.start();
    mConnection->moveToThread(&mThread);

    connect(mConnection.data(), &Socket::deviceName, this, &Transfer::setDeviceName);
    connect(mConnection.data(), &Socket::progress, this, &Transfer::setProgress);
    connect(mConnection.data(), &Socket::error, this, &Transfer::error);
    connect(mConnection.data(), &Socket::completed, this, &Transfer::completed);

    connect(mConnection.data(), &Socket::deviceName, this, &Transfer::statusChanged);
    connect(mConnection.data(), &Socket::progress, this, &Transfer::statusChanged);
    connect(mConnection.data(), &Socket::error, this, &Transfer::finished);
    connect(mConnection.data(), &Socket::completed, this, &Transfer::finished);
}
