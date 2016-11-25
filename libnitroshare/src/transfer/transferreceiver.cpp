/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Nathan Osman
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

#include <nitroshare/transfermodel.h>
#include <nitroshare/transport.h>

#include "transferreceiver.h"

TransferReceiver::TransferReceiver(Transport *transport)
    : Transfer(TransferModel::Receive),
      mNextPacketSize(InvalidSize)
{
    connect(transport, &Transport::dataReceived, this, &TransferReceiver::onDataReceived);
}

void TransferReceiver::onDataReceived(const QByteArray &data)
{
    mReadBuffer.append(data);

    // Continue processing complete packets until none remain in the buffer
    forever {

        // Ignore any packets if transfer has completed
        if (mProtocolState == Finished) {
            break;
        }

        // If mNextPacketSize is unset, attempt to read it
        if (mNextPacketSize == InvalidSize) {

            // Determine if there is enough data in the buffer to read the size
            if (static_cast<size_t>(mReadBuffer.size()) >= sizeof(mNextPacketSize)) {

                // memcpy must be used in order to avoid alignment issues
                memcpy(&mNextPacketSize, mReadBuffer.constData(), sizeof(mNextPacketSize));

                // Byte order is little-endian by default - ensure conformance
                mNextPacketSize = qFromLittleEndian(mNextPacketSize);

                // Remove the size from the beginning of the packet
                mReadBuffer.remove(0, sizeof(mNextPacketSize));
            } else {
                break;
            }
        }

        // If the buffer contains enough data, process the packet
        if (mReadBuffer.size() >= mNextPacketSize) {
            processPacket(mReadBuffer.left(mNextPacketSize));
            mReadBuffer.remove(0, mNextPacketSize);
            mNextPacketSize = InvalidSize;
        }
    }
}

void TransferReceiver::processPacket(const QByteArray &packet)
{
    //...
}
