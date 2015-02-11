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

#include <cstring>

#include <QtEndian>

#include "socket.h"

Socket::Socket()
    : mBufferSize(0)
{
    connect(this, &Socket::readyRead, this, &Socket::processRead);
    connect(this, &Socket::bytesWritten, this, &Socket::processWrite);
}

void Socket::processRead()
{
    mBuffer.append(readAll());

    // Attempt to read all available packets from the buffer
    forever {
        // If the size of the packet is not yet known try to read it
        if(!mBufferSize) {
            if(mBuffer.size() >= sizeof(mBufferSize)) {
                memcpy(&mBufferSize, mBuffer.constData(), sizeof(mBufferSize));
                mBufferSize = qFromLittleEndian(mBufferSize);
                mBuffer.remove(0, sizeof(mBufferSize));
            } else {
                break;
            }
        }

        // The size is known at this point, check to see if the amount is available
        if(mBuffer.size() >= mBufferSize) {
            processPacket(qUncompress(mBuffer.left(mBufferSize)));
            mBuffer.remove(0, mBufferSize);
            mBufferSize = 0;
        } else {
            break;
        }
    };
}

void Socket::processWrite()
{
    // Only write the next packet when the buffer is empty
    if(!bytesToWrite()) {
        writeNextPacket();
    }
}

void Socket::writePacket(const QByteArray &data)
{
    // Write the length of the packet followed by the contents
    qint32 size = qToLittleEndian(data.length());
    write(reinterpret_cast<const char*>(size), sizeof(size));
    write(qCompress(data));
}
