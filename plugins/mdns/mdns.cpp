/*
 * The MIT License (MIT)
 *
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

#include <QtGlobal>

#ifdef Q_OS_LINUX
#  include <sys/socket.h>
#endif

#include <nitroshare/logger.h>

#include "mdns.h"

const QString Tag = "mdns";

Mdns::Mdns(Logger *logger)
    : mLogger(logger)
{
    connect(&mIpv4Socket, &QUdpSocket::readyRead, this, &Mdns::onReadyRead);
    connect(&mIpv6Socket, &QUdpSocket::readyRead, this, &Mdns::onReadyRead);

    bind(mIpv4Socket, QHostAddress::AnyIPv4);
    bind(mIpv6Socket, QHostAddress::AnyIPv6);
}

void Mdns::onReadyRead()
{
    qDebug() << "Packet received!";
}

void Mdns::bind(QUdpSocket &socket, const QHostAddress &address)
{
    // For some reason, ShareAddress doesn't always work on Linux, so bind()
    // must be called twice with a call to setsockopt() in the middle

    if (!socket.bind(address, 5353, QAbstractSocket::ShareAddress | QAbstractSocket::ReuseAddressHint)) {
#ifdef Q_OS_LINUX
        int arg = 1;
        if (setsockopt(socket.socketDescriptor(), SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char*>(&arg), sizeof(arg)) ||
                !socket.bind(address, 5353, QAbstractSocket::ShareAddress | QAbstractSocket::ReuseAddressHint)) {
#endif
            mLogger->error(Tag, socket.errorString());
#ifdef Q_OS_LINUX
        }
#endif
    }
}
