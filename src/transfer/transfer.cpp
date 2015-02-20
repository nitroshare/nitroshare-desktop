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

#include <QJsonDocument>
#include <QJsonObject>
#include <QtEndian>

#include "transfer.h"

Transfer::Transfer(TransferModel::Direction direction)
    : mDirection(direction)
{
    connect(&mSocket, &QTcpSocket::connected, this, &Transfer::onConnected);
    connect(&mSocket, &QTcpSocket::readyRead, this, &Transfer::onReadyRead);
    connect(&mSocket, &QTcpSocket::bytesWritten, this, &Transfer::onBytesWritten);

    // The error() method is overloaded (sigh) so we need to be very explicit here
    connect(&mSocket, static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>(&QTcpSocket::error), this, &Transfer::onError);

    // Set all of the transfer members to proper initial values
    reset();
}

void Transfer::cancel()
{
    // Ensure that the transfer is actually progress before trying to abort it
    if(mState == TransferModel::Canceled || mState == TransferModel::Failed || mState == TransferModel::Succeeded) {
        qWarning("Cannot cancel a transfer that has completed");
        return;
    }

    mSocket.abort();

    mState = TransferModel::Canceled;
    emit dataChanged();
}

void Transfer::restart()
{
    // Ensure that the transfer is not in progress before restarting it
    if(mState == TransferModel::Connecting || mState == TransferModel::InProgress) {
        qWarning("Cannot start a transfer that is in progress");
        return;
    }

    reset();
    start();
}

void Transfer::onConnected()
{
    mState = TransferModel::InProgress;
    emit dataChanged();
}

void Transfer::onReadyRead()
{
    // Add all of the new data to the buffer
    mBuffer.append(mSocket.readAll());

    // Process as many packets as can be read from the buffer
    forever {
        // If the size of the packet is not yet known attempt to read it
        if(!mBufferSize) {
            if(static_cast<size_t>(mBuffer.size()) >= sizeof(mBufferSize)) {
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
            QByteArray data = mBuffer.left(mBufferSize);
            mBuffer.remove(0, mBufferSize);
            mBufferSize = 0;

            // Pass the packet along to the child class
            processPacket(data);
        } else {
            break;
        }
    };
}

void Transfer::onBytesWritten()
{
    // This slot is invoked after data has been written - only write more
    // packets if there is no data still waiting to be written
    if(!mSocket.bytesToWrite()) {
        writeNextPacket();
    }
}

void Transfer::onError(QAbstractSocket::SocketError)
{
    // Errors are only meaningful during the Connecting and InProgress states
    if(mState == TransferModel::Connecting || mState == TransferModel::InProgress) {
        abortWithError(mSocket.errorString());
    }
}

void Transfer::writePacket(const QByteArray &data)
{
    // Write the length of the data and its contents
    qint32 packetSize = qToLittleEndian(data.length());
    mSocket.write(reinterpret_cast<const char*>(&packetSize), sizeof(packetSize));
    mSocket.write(data);
}

void Transfer::writePacket(const QVariantMap &map)
{
    // Create a JSON string from the map and write it
    QJsonObject object = QJsonObject::fromVariantMap(map);
    writePacket(QJsonDocument(object).toJson(QJsonDocument::Compact));
}

void Transfer::calculateProgress()
{
    // Store the old value to avoid emiting the dataChanged signal more often than necessary
    int oldProgress = mProgress;

    // Calculate the current progress in the range 0-100,
    // being careful to avoid a division by 0 error
    if(mTransferBytesTotal) {
        double n = static_cast<double>(mTransferBytes),
               d = static_cast<double>(mTransferBytesTotal);
        mProgress = static_cast<int>((n / d) * 100.0);
    } else {
        mProgress = 0;
    }

    if(mProgress != oldProgress) {
        emit dataChanged();
    }
}

void Transfer::abortWithError(const QString &message)
{
    // This method is (thankfully) idempotent
    mSocket.abort();

    mError = message;
    mState = TransferModel::Failed;
    emit dataChanged();
}

void Transfer::finish()
{
    mSocket.abort();

    mState = TransferModel::Succeeded;
    emit dataChanged();
}

void Transfer::reset()
{
    // Reset all of the state variables to proper default values
    mProtocolState = TransferHeader;

    mTransferBytes = 0;
    mTransferBytesTotal = 0;

    // The state is already in progress for a receiving transfer
    mState = mDirection == TransferModel::Send ? TransferModel::Connecting : TransferModel::InProgress;

    mProgress = 0;

    mBuffer.clear();
    mBufferSize = 0;
}
