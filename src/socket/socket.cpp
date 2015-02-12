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

void Socket::start()
{
    // Reset transfer variables
    mTransferBytes = 0;
    mTransferBytesTotal = 0;

    initialize();
}

void Socket::processRead()
{
    // Add the new data to the buffer
    mBuffer.append(readAll());

    // Process as many packets as can be read from the buffer
    forever {
        // If the size of the packet is not yet known attempt to read it
        if(!mBufferSize) {
            if(mBuffer.size() >= sizeof(mBufferSize)) {
                // memcpy must be used in order to avoid alignment issues
                memcpy(&mBufferSize, mBuffer.constData(), sizeof(mBufferSize));
                mBufferSize = qFromLittleEndian(mBufferSize);
                mBuffer.remove(0, sizeof(mBufferSize));
            } else {
                break;
            }
        }

        // At this point, the size of the packet is known,
        // check if the specified number of bytes are available
        if(mBuffer.size() >= mBufferSize) {
            // Read the data and uncompress it
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
    // This slot is invoked after data has been written - only write more
    // packets if there is no data still waiting to be written
    if(!bytesToWrite()) {
        writeNextPacket();
    }
}

void Socket::writePacket(const QByteArray &data)
{
    // Compress the data, writing its length and contents
    QByteArray compressed = qCompress(data);
    qint32 packetSize = qToLittleEndian(compressed.length());
    write(reinterpret_cast<const char*>(&packetSize), sizeof(packetSize));
    write(compressed);
}

void Socket::emitProgress()
{
    // Calculate the current progress in the range 0-100,
    // being sure to avoid a division by 0 error
    if(mTransferBytesTotal) {
        double n = static_cast<double>(mTransferBytes),
               d = static_cast<double>(mTransferBytesTotal);
        emit progress(static_cast<int>((n / d) * 100.0));
    } else {
        emit progress(0);
    }
}
