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

#include "../socket/socketreceiver.h"
#include "../socket/socketsender.h"
#include "transfer.h"
#include "transfer_p.h"

TransferPrivate::TransferPrivate(Socket *socket, Transfer *transfer)
    : QObject(transfer),
      q(transfer),
      socket(socket),
      progress(0)
{
    connect(socket, &Socket::connected, this, &TransferPrivate::processConnect);

    // The static_cast<> is necessary since error() is overloaded
    connect(socket, static_cast<void (Socket::*)(QAbstractSocket::SocketError)>(&Socket::error),
            this, &TransferPrivate::processError);

    connect(socket, &Socket::deviceName, this, &TransferPrivate::setDeviceName);
    connect(socket, &Socket::progress, this, &TransferPrivate::setProgress);
    connect(socket, &Socket::transferError, this, &TransferPrivate::setError);
    connect(socket, &Socket::success, this, &TransferPrivate::setSuccess);
}

TransferPrivate::~TransferPrivate()
{
    delete socket;
}

void TransferPrivate::processConnect()
{
    // Only SocketSender instances will invoke this slot since
    // SocketReceiver instances begin in the connected state
    emit q->stateChanged(state = Transfer::InProgress);
}

void TransferPrivate::processError()
{
    // We only deal with errors that occur during Connecting and InProgress
    // states since errors no longer matter in any of the other states
    if(state == Transfer::Connecting || state == Transfer::InProgress) {
        error = socket->errorString();
        emit q->stateChanged(state = Transfer::Failed);
    }
}

void TransferPrivate::setDeviceName(const QString &value)
{
    emit q->deviceNameChanged(deviceName = value);
}

void TransferPrivate::setProgress(int value)
{
    emit q->progressChanged(progress = value);
}

void TransferPrivate::setError(const QString &value)
{
    // Change the state before disconnecting to ensure that any
    // errors that arise from aborting the connection are ignored
    error = value;
    emit q->stateChanged(state = Transfer::Failed);
    socket->abort();
}

void TransferPrivate::setSuccess()
{
    emit q->stateChanged(state = Transfer::Succeeded);
    socket->abort();
}

Transfer::Transfer(qintptr socketDescriptor, QObject *parent)
    : QObject(parent),
      d(new TransferPrivate(new SocketReceiver(socketDescriptor), this))
{
    d->direction = Receive;
    d->state = InProgress;
    d->deviceName = tr("[unknown]");
}

Transfer::Transfer(const Device *device, BundlePointer bundle, QObject *parent)
    : QObject(parent),
      d(new TransferPrivate(new SocketSender(device, bundle), this))
{
    d->direction = Send;
    d->state = Connecting;
    d->deviceName = device->name();

    d->socket->start();
}

Transfer::Direction Transfer::direction() const
{
    return d->direction;
}

Transfer::State Transfer::state() const
{
    return d->state;
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

void Transfer::cancel()
{
    if(d->state == Canceled || d->state == Failed || d->state == Succeeded) {
        qWarning("Cannot cancel a transfer that has finished");
        return;
    }

    emit stateChanged(d->state = Transfer::Canceled);
    d->socket->abort();
}

void Transfer::restart()
{
    if(d->direction == Receive) {
        qWarning("Cannot restart a transfer that receives files");
        return;
    }

    if(d->state == Connecting || d->state == InProgress) {
        qWarning("Cannot restart a transfer in progress");
        return;
    }

    d->socket->start();
}
