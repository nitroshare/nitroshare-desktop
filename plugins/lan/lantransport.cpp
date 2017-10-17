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

#include <cstring>

#include <QtEndian>

#include <nitroshare/packet.h>

#include "lantransport.h"

LanTransport::LanTransport(const QHostAddress &address, quint16 port)
    : LanTransport()
{
    connect(&mSocket, &QTcpSocket::connected, this, &LanTransport::onConnected);

    mSocket.connectToHost(address, port);
}

LanTransport::LanTransport(qintptr socketDescriptor)
    : LanTransport()
{
    mSocket.setSocketDescriptor(socketDescriptor);
}

void LanTransport::sendPacket(Packet *packet)
{
    // Build the parts of the packet
    QByteArray content = packet->content();
    qint32 packetSize = qToLittleEndian(content.size() + 1);
    qint8 packetType = packet->type();

    // Send the length and type of the packet
    mSocket.write(reinterpret_cast<const char*>(&packetSize), sizeof(packetSize));
    mSocket.write(reinterpret_cast<const char*>(&packetType), sizeof(packetType));

    // If the packet includes content, send it too
    if (content.length()) {
        mSocket.write(content);
    }
}

void LanTransport::close()
{
    mSocket.close();
}

void LanTransport::onConnected()
{
    emit connected();
}

void LanTransport::onReadyRead()
{
    mBuffer.append(mSocket.readAll());

    // Continue to emit packets as they are read
    while (mBuffer.size()) {
        if (mBufferSize) {

            // Only continue if the buffer has the full packet
            if (mBuffer.size() < mBufferSize) {
                break;
            }

            // Grab the type and data
            const char type = mBuffer.at(0);
            QByteArray data = mBuffer.remove(1, mBufferSize - 1);

            // Emit the new packet and reset the size
            emit packetReceived(new Packet(static_cast<Packet::Type>(type), data));
            mBufferSize = 0;

        } else {

            // Only continue if the buffer has enough data for the size
            if (mBuffer.size() < sizeof(mBufferSize)) {
                break;
            }

            // memcpy must be used in order to avoid alignment issues
            memcpy(&mBufferSize, mBuffer.constData(), sizeof(mBufferSize));
            mBufferSize = qFromLittleEndian(mBufferSize);
            mBuffer.remove(0, sizeof(mBufferSize));

            // A packet size of zero is an error (and impossible)
            if (!mBufferSize) {
                emit error(tr("invalid packet received"));
                break;
            }
        }
    }
}

void LanTransport::onBytesWritten()
{
    // TODO: packet doesn't necessarily correspond with bytes being written,
    // so this really should be rewritten to take that into account

    emit packetSent();
}

void LanTransport::onError(QAbstractSocket::SocketError)
{
    emit error(mSocket.errorString());
}

LanTransport::LanTransport()
    : mBufferSize(0)
{
    connect(&mSocket, &QTcpSocket::readyRead, this, &LanTransport::onReadyRead);
    connect(&mSocket, &QTcpSocket::bytesWritten, this, &LanTransport::onBytesWritten);
    connect(&mSocket, static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>(&QTcpSocket::error), this, &LanTransport::onError);
}
