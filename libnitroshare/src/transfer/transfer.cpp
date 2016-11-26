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

#include <nitroshare/transfer.h>
#include <nitroshare/transport.h>

#include "transfer_p.h"

TransferPrivate::TransferPrivate(Transfer *parent, HandlerRegistry *handlerRegistry, Transport *transport, Transfer::Direction direction)
    : QObject(parent),
      q(parent),
      handlerRegistry(handlerRegistry),
      transport(transport),
      protocolState(TransferHeader),
      direction(direction),
      state(Transfer::Connecting),
      progress(0),
      nextPacketSize(0)
{
    connect(transport, &Transport::dataReceived, this, &TransferPrivate::onDataReceived);
}

void TransferPrivate::processPacket(const QByteArray &packet)
{
    if (direction == Transfer::Send) {
        //...
    } else {
        switch (protocolState) {
        case TransferHeader:
            // processTransferHeader(packet);
            return;
        case ItemHeader:
            // processItemHeader(packet);
            return;
        case Item:
            // processItem(packet);
            return;
        }
    }

    // The packet was unexpected - assume an error and ignore it
    protocolState = Finished;
    emit q->stateChanged(state = Transfer::Failed);
    emit q->errorChanged(error = tr("protocol error - unexpected packet"));
}

void TransferPrivate::onDataReceived(const QByteArray &data)
{
    readBuffer.append(data);

    // Continue processing complete packets until none remain in the buffer
    forever {

        // Ignore any packets if transfer has completed
        if (protocolState == Finished) {
            break;
        }

        // If mNextPacketSize is unset, attempt to read it
        if (!nextPacketSize) {

            // Determine if there is enough data in the buffer to read the size
            if (static_cast<size_t>(readBuffer.size()) >= sizeof(nextPacketSize)) {

                // memcpy must be used in order to avoid alignment issues
                memcpy(&nextPacketSize, readBuffer.constData(), sizeof(nextPacketSize));

                // Byte order is little-endian by default - ensure conformance
                nextPacketSize = qFromLittleEndian(nextPacketSize);

                // Remove the size from the beginning of the packet
                readBuffer.remove(0, sizeof(nextPacketSize));
            } else {
                break;
            }
        }

        // If the buffer contains enough data, remove it and process the packet
        if (readBuffer.size() >= nextPacketSize) {
            processPacket(readBuffer.left(nextPacketSize));
            readBuffer.remove(0, nextPacketSize);
            nextPacketSize = 0;
        }
    }
}

Transfer::Transfer(HandlerRegistry *handlerRegistry, Transport *transport, QObject *parent)
    : QObject(parent),
      d(new TransferPrivate(this, handlerRegistry, transport, Transfer::Receive))
{
}

Transfer::Transfer(HandlerRegistry *handlerRegistry, Transport *transport, Bundle *bundle, QObject *parent)
    : QObject(parent),
      d(new TransferPrivate(this, handlerRegistry, transport, Transfer::Send))
{
}

Transfer::Direction Transfer::direction() const
{
    return d->direction;
}

Transfer::State Transfer::state() const
{
    return d->state;
}

int Transfer::progress() const
{
    return d->progress;
}

QString Transfer::deviceName() const
{
    return d->deviceName;
}

QString Transfer::error() const
{
    return d->error;
}

void Transfer::abort()
{
}

void Transfer::restart()
{
}
