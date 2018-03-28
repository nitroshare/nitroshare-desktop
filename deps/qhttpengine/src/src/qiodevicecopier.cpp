/*
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

#include <QIODevice>
#include <QTimer>

#include <qhttpengine/qiodevicecopier.h>

#include "qiodevicecopier_p.h"

using namespace QHttpEngine;

// Default value for the bufferSize property
const qint64 DefaultBufferSize = 65536;

QIODeviceCopierPrivate::QIODeviceCopierPrivate(QIODeviceCopier *copier, QIODevice *srcDevice, QIODevice *destDevice)
    : QObject(copier),
      q(copier),
      src(srcDevice),
      dest(destDevice),
      bufferSize(DefaultBufferSize),
      rangeFrom(0),
      rangeTo(-1)
{
}

void QIODeviceCopierPrivate::onReadyRead()
{
    if (dest->write(src->readAll()) == -1) {
        Q_EMIT q->error(dest->errorString());
        src->close();
    }
}

void QIODeviceCopierPrivate::onReadChannelFinished()
{
    // Read any data that remains and signal the end of the operation
    if (src->bytesAvailable()) {
        onReadyRead();
    }

    Q_EMIT q->finished();
}

void QIODeviceCopierPrivate::nextBlock()
{
    // Attempt to read an amount of data up to the size of the buffer
    QByteArray data;
    data.resize(bufferSize);
    qint64 dataRead = src->read(data.data(), bufferSize);

    // If an error occurred during the read, emit an error
    if (dataRead == -1) {
        Q_EMIT q->error(src->errorString());
        Q_EMIT q->finished();
        return;
    }

    // If range is specified (rangeTo >= 0), check if end of range is reached;
    // if it is, send only part from buffer truncated by range end
    if (rangeTo != -1 && src->pos() > rangeTo) {
        dataRead -= src->pos() - rangeTo - 1;
    }

    // Write the data to the destination device
    if (dest->write(data.constData(), dataRead) == -1) {
        Q_EMIT q->error(dest->errorString());
        Q_EMIT q->finished();
        return;
    }

    // Check if the end of the device has been reached or if the end of
    // the requested range is reached - if so, emit the finished signal and
    // if not, continue to read data at the next iteration of the event loop
    if (src->atEnd() || (rangeTo != -1 && src->pos() > rangeTo)) {
        Q_EMIT q->finished();
    } else {
        QTimer::singleShot(0, this, &QIODeviceCopierPrivate::nextBlock);
    }
}

QIODeviceCopier::QIODeviceCopier(QIODevice *src, QIODevice *dest, QObject *parent)
    : QObject(parent),
      d(new QIODeviceCopierPrivate(this, src, dest))
{
    connect(src, &QIODevice::destroyed, this, &QIODeviceCopier::stop);
    connect(dest, &QIODevice::destroyed, this, &QIODeviceCopier::stop);
}

void QIODeviceCopier::setBufferSize(qint64 size)
{
    d->bufferSize = size;
}

void QIODeviceCopier::setRange(qint64 from, qint64 to)
{
    d->rangeFrom = from;
    d->rangeTo = to;
}

void QIODeviceCopier::start()
{
    if (!d->src->isOpen()) {
        if (!d->src->open(QIODevice::ReadOnly)) {
            Q_EMIT error(tr("Unable to open source device for reading"));
            Q_EMIT finished();
            return;
        }
    }

    if (!d->dest->isOpen()) {
        if (!d->dest->open(QIODevice::WriteOnly)) {
            Q_EMIT error(tr("Unable to open destination device for writing"));
            Q_EMIT finished();
            return;
        }
    }

    // If range is set and d->src is not sequential, seek to starting position
    if (d->rangeFrom > 0 && !d->src->isSequential()) {
        if (!d->src->seek(d->rangeFrom)) {
            Q_EMIT error(tr("Unable to seek source device for specified range"));
            Q_EMIT finished();
            return;
        }
    }

    // These signals cannot be connected in the constructor since they may
    // begin firing before the start() method is called

    // readyRead() and readChannelFinished() are only emitted for sequential
    // devices - for other types of devices, it is necessary to check atEnd()
    // in order to determine whether the end of the device has been reached
    connect(d->src, &QIODevice::readyRead, d, &QIODeviceCopierPrivate::onReadyRead);
    connect(d->src, &QIODevice::readChannelFinished, d, &QIODeviceCopierPrivate::onReadChannelFinished);

    // The first read from the device needs to be triggered
    QTimer::singleShot(0, d, d->src->isSequential() ?
            &QIODeviceCopierPrivate::onReadyRead :
            &QIODeviceCopierPrivate::nextBlock);
}

void QIODeviceCopier::stop()
{
    disconnect(d->src, &QIODevice::readyRead, d, &QIODeviceCopierPrivate::onReadyRead);
    disconnect(d->src, &QIODevice::readChannelFinished, d, &QIODeviceCopierPrivate::onReadChannelFinished);

    Q_EMIT finished();
}
