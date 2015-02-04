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
#include "transfer_p.h"

TransferPrivate::TransferPrivate(Transfer *transfer,
                                 Socket *socket,
                                 const QString &deviceName,
                                 Transfer::Direction direction)
    : QObject(transfer),
      q(transfer),
      socket(socket),
      deviceName(deviceName),
      progress(0),
      status(Transfer::Pending),
      direction(direction)
{
    thread.start();
    socket->moveToThread(&thread);

    // Store the information made available by the socket through signals
    connect(socket, &Socket::deviceNameChanged, this, &TransferPrivate::setDeviceName);
    connect(socket, &Socket::progressChanged, this, &TransferPrivate::setProgress);
    connect(socket, &Socket::error, this, &TransferPrivate::setStatusError);
    connect(socket, &Socket::completed, this, &TransferPrivate::setStatusCompleted);
}

TransferPrivate::~TransferPrivate()
{
    // If a transfer is in progress, canceling it will cause
    // either the error or completed signal to be emitted
    if(status == Transfer::InProgress) {
        q->cancel();
    }

    // Wait for the thread to finish (this should never block for more than a few MS)
    thread.quit();
    thread.wait();

    delete socket;
}

void TransferPrivate::setDeviceName(const QString &value)
{
    deviceName = value;
    emit q->deviceNameChanged(deviceName);
}

void TransferPrivate::setProgress(int value)
{
    progress = value;
    emit q->progressChanged(progress);
}

void TransferPrivate::setStatusError(const QString &value)
{
    status = Transfer::Error;
    error = value;
    emit q->statusChanged(status);
}

void TransferPrivate::setStatusCompleted()
{
    status = Transfer::Completed;
    emit q->statusChanged(status);
}

Transfer::Transfer(qintptr socketDescriptor)
    : d(new TransferPrivate(this, new SocketReader(socketDescriptor), tr("[unknown]"), Receive))
{
}

Transfer::Transfer(DevicePointer device, BundlePointer bundle)
    : d(new TransferPrivate(this, new SocketWriter(device, bundle), device->name(), Send))
{
}

QString Transfer::deviceName() const
{
    return d->deviceName;
}

int Transfer::progress() const
{
    return d->progress;
}

QString Transfer::error() const
{
    return d->error;
}

Transfer::Status Transfer::status() const
{
    return d->status;
}

Transfer::Direction Transfer::direction() const
{
    return d->direction;
}

void Transfer::start()
{
    if(d->status == InProgress) {
        qWarning("Cannot start a transfer that is already in progress");
        return;
    }

    QMetaObject::invokeMethod(d->socket, "start", Qt::QueuedConnection);

    d->status = InProgress;
    emit statusChanged(d->status);
}

void Transfer::cancel()
{
    if(d->status != InProgress) {
        qWarning("Cannot cancel a transfer that is not in progress");
        return;
    }

    QMetaObject::invokeMethod(d->socket, "cancel", Qt::QueuedConnection);
}
