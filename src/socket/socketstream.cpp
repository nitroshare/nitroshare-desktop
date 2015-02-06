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

#include <QEventLoop>

#include "../util/settings.h"
#include "socketstream.h"
#include "socketstream_p.h"

SocketStreamPrivate::SocketStreamPrivate(QTcpSocket *socket, SocketStream *stream)
    : QObject(stream),
      q(stream),
      socket(socket)
{
    // This will only be emitted if waitForConnected was called and
    // therefore signal will always be equal to Connected
    connect(socket, &QTcpSocket::connected, [this]() {
        succeeded = true;
        emit quitRequested();
    });

    connect(socket, &QTcpSocket::readyRead, [this]() {
        if(signal == ReadyRead) {
            succeeded = true;
            emit quitRequested();
        }
    });

    connect(socket, &QTcpSocket::bytesWritten, [this]() {
        if(signal == BytesWritten) {
            succeeded = true;
            emit quitRequested();
        }
    });

    // Both a disconnect and a timeout are always unexpected when our event
    // loop is active and are therefore errors
    connect(socket, &QTcpSocket::disconnected, this, &SocketStreamPrivate::quitRequested);
    connect(&timer, &QTimer::timeout, this, &SocketStreamPrivate::quitRequested);

    timer.setSingleShot(true);
    timer.setInterval(Settings::get<int>(Settings::TransferTimeout));
}

bool SocketStreamPrivate::waitFor(Signal value)
{
    signal = value;
    succeeded = false;

    // Start the timer that will timeout if the operation takes too long
    timer.start();

    // Create an event loop that will run until quitRequested is emitted
    QEventLoop eventLoop;
    connect(this, &SocketStreamPrivate::quitRequested, &eventLoop, &QEventLoop::quit);
    eventLoop.exec();

    timer.stop();

    return succeeded;
}

SocketStream::SocketStream(QTcpSocket *socket)
    : d(new SocketStreamPrivate(socket, this))
{
}

bool SocketStream::waitForConnected()
{
    return d->waitFor(SocketStreamPrivate::Connected);
}

void SocketStream::read(char *data, qint32 length)
{
    while(length) {
        qint64 lengthRead = d->socket->read(data, length);

        if(lengthRead == -1) {
            throw tr("Unable to read from socket.");
        }

        data += lengthRead;
        length -= lengthRead;

        if(length && !d->waitFor(SocketStreamPrivate::ReadyRead)) {
            throw tr("Timeout while reading from socket.");
        }
    }
}

QByteArray SocketStream::readQByteArray()
{
    qint32 length = readInt<qint32>();

    QByteArray value;
    value.resize(length);

    read(value.data(), length);

    return value;
}

void SocketStream::write(const char *data, qint32 length)
{
    if(d->socket->write(data, length) == -1) {
        throw tr("Unable to write to socket.");
    }

    // TODO: waiting here may be a bottleneck
    // further investigation is needed

    while(d->socket->bytesToWrite()) {
        if(!d->waitFor(SocketStreamPrivate::BytesWritten)) {
            throw tr("Timeout while writing to socket.");
        }
    }
}

void SocketStream::writeQByteArray(const QByteArray &value)
{
    writeInt<qint32>(value.length());
    write(value.constData(), value.length());
}

void SocketStream::abort()
{
    emit d->quitRequested();
}
