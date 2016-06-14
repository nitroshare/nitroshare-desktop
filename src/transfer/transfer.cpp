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
#include <QSslSocket>
#include <QtEndian>
#include <QVariantMap>

#include "../util/json.h"
#include "transfer.h"

Transfer::Transfer(QSslConfiguration *configuration, TransferModel::Direction direction)
    : mDirection(direction)
{
    // Create a socket of the appropriate type
    if (configuration) {
        QSslSocket *socket = new QSslSocket(this);
        socket->setSslConfiguration(*configuration);

        connect(socket, &QSslSocket::encrypted, this, &Transfer::onEncrypted);
        connect(socket, static_cast<void(QSslSocket::*)(const QList<QSslError> &)>(&QSslSocket::sslErrors), this, &Transfer::onSslErrors);

        mSocket = socket;
    } else {
        mSocket = new QTcpSocket(this);
    }

    connect(mSocket, &QTcpSocket::connected, this, &Transfer::onConnected);
    connect(mSocket, &QTcpSocket::readyRead, this, &Transfer::onReadyRead);
    connect(mSocket, &QTcpSocket::bytesWritten, this, &Transfer::onBytesWritten);
    connect(mSocket, static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>(&QTcpSocket::error), this, &Transfer::onError);

    // Set all of the transfer members to proper initial values
    reset();
}

void Transfer::cancel()
{
    // Ensure that the transfer is actually in progress before trying to cancel it
    if(mState == TransferModel::Failed || mState == TransferModel::Succeeded) {
        qWarning("Cannot cancel a transfer that has completed");
        return;
    }

    // Canceling a request is considered an "error" and is treated the same way
    // Only attempt to report the error if the transfer isn't already finished
    if(mProtocolState != ProtocolState::Finished) {
        writeErrorPacket(tr("Transfer was canceled"));
    }
}

void Transfer::restart()
{
    // Ensure that the transfer is sending data and not receiving
    if(mDirection == TransferModel::Receive) {
        qWarning("Cannot restart a transfer that receives files");
        return;
    }

    // Ensure that the transfer failed
    if(mState != TransferModel::Failed) {
        qWarning("Cannot restart a transfer that has not failed");
        return;
    }

    // Reset all of the transfer variables and reconnect again
    reset();
    startConnect();
}

void Transfer::onEncrypted()
{
    mState = TransferModel::InProgress;
    emit dataChanged({TransferModel::StateRole});

    startTransfer();
}

void Transfer::onConnected()
{
    QSslSocket *socket = qobject_cast<QSslSocket*>(mSocket);
    if (socket) {
        if (mDirection == TransferModel::Send) {
            socket->startClientEncryption();
        } else {
            socket->startServerEncryption();
        }
    } else {
        startTransfer();
    }
}

void Transfer::onReadyRead()
{
    // Add all of the new data to the buffer
    mBuffer.append(mSocket->readAll());

    // Process as many packets as can be read from the buffer
    forever {

        // If the transfer is finished, ignore any packets being received
        if(mProtocolState == ProtocolState::Finished) {
            break;
        }

        // If the size of the packet is not yet known attempt to read it
        if(!mBufferSize) {

            // See if there is enough data in the buffer to read the size
            if(static_cast<size_t>(mBuffer.size()) >= sizeof(mBufferSize)) {

                // memcpy must be used in order to avoid alignment issues
                // Also, the integer uses little endian byte order
                // so convert it to the host format if necessary
                memcpy(&mBufferSize, mBuffer.constData(), sizeof(mBufferSize));
                mBufferSize = qFromLittleEndian(mBufferSize);
                mBuffer.remove(0, sizeof(mBufferSize));

                // A packet size of zero is an error
                if(!mBufferSize) {
                    writeErrorPacket(tr("Empty packet received"));
                    break;
                }

            } else {
                break;
            }
        }

        // If the buffer contains enough data to read the packet, then do so
        if(mBuffer.size() >= mBufferSize) {
            processPacket();
        } else {
            break;
        }
    }
}

void Transfer::onBytesWritten()
{
    // Wait until there is no more pending data to write
    if(!mSocket->bytesToWrite()) {

        // If the transfer finished, then report success or failure
        // Otherwise, have the child class write the next packet
        if(mProtocolState == ProtocolState::Finished) {
            finish(mError.isNull() ? TransferModel::Succeeded : TransferModel::Failed);
        } else {
            writeNextPacket();
        }
    }
}

void Transfer::onError(QAbstractSocket::SocketError)
{
    // Errors are only meaningful during the Connecting and InProgress
    if(mState == TransferModel::Connecting || mState == TransferModel::InProgress) {
        finish(TransferModel::Failed);
    }
}

void Transfer::onSslErrors(const QList<QSslError> &errors)
{
    // Ignore HostNameMismatch errors since certificates are automatically
    // valid if they are signed by the CA - show all other errors
    if (errors.count() == 1 && errors.at(0).error() == QSslError::HostNameMismatch) {
        qobject_cast<QSslSocket*>(mSocket)->ignoreSslErrors();
    } else {
        mError = errors.at(0).errorString();
        finish(TransferModel::Failed);
    }
}

void Transfer::writeSuccessPacket()
{
    // A success packet contains absolutely no data
    writePacket(PacketType::Success);
    mProtocolState = ProtocolState::Finished;
}

void Transfer::writeErrorPacket(const QString &message)
{
    // Send the error as a UTF-8 encoded string
    writePacket(PacketType::Error, message.toUtf8());

    // Record the error message and emit the appropriate signal
    mError = message;
    emit dataChanged({TransferModel::ErrorRole});

    // Indicate that the transfer has finished so that all future packets are ignored
    mProtocolState = ProtocolState::Finished;
}

void Transfer::writeJsonPacket(const QJsonObject &object)
{
    // Convert the JSON object to a byte array and send it
    writePacket(PacketType::Json, QJsonDocument(object).toJson(QJsonDocument::Compact));
}

void Transfer::writeBinaryPacket(const QByteArray &data)
{
    // Pass the data along unmodified
    writePacket(PacketType::Binary, data);
}

void Transfer::updateProgress()
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

    // Ensure that the value falls within range
    mProgress = qMin(qMax(mProgress, 0), 100);

    // Only emit a signal if the value changes
    if(mProgress != oldProgress) {
        emit dataChanged({TransferModel::ProgressRole});
    }
}

void Transfer::processPacket()
{
    // Grab the data from the front of the buffer
    const char type = mBuffer.at(0);
    QByteArray data = mBuffer.mid(1, mBufferSize - 1);
    mBuffer.remove(0, mBufferSize);
    mBufferSize = 0;

    // Process the data based on the type
    switch(static_cast<PacketType>(type)) {
    case PacketType::Success:
    {
        finish(TransferModel::Succeeded);
        break;
    }
    case PacketType::Error:
    {
        // An error occurred - grab the error message
        mError = QString::fromUtf8(data);
        emit dataChanged({TransferModel::ErrorRole});

        finish(TransferModel::Failed);
        break;
    }
    case PacketType::Json:
    {
        // Verify the JSON is valid and pass it along
        QJsonDocument document = QJsonDocument::fromJson(data);
        if(document.isObject()) {
            processJsonPacket(document.object());
        } else {
            writeErrorPacket(tr("Unable to read JSON packet"));
        }
        break;
    }
    case PacketType::Binary:
    {
        // Pass the packet along unmodified
        processBinaryPacket(data);
        break;
    }
    default:
        // Any other type of packet is an error
        writeErrorPacket(tr("Unrecognized packet received"));
    }
}

void Transfer::writePacket(PacketType type, const QByteArray &data)
{
    // Write the size of the packet including its type
    qint32 packetSize = qToLittleEndian(data.length() + 1);
    mSocket->write(reinterpret_cast<const char*>(&packetSize), sizeof(packetSize));

    // Write the packet type and the data (if provided)
    mSocket->write(reinterpret_cast<const char*>(&type), 1);
    if(data.length()) {
        mSocket->write(data);
    }
}

void Transfer::reset()
{
    // Reset all of the state variables to their initial values
    mProtocolState = ProtocolState::TransferHeader;

    mTransferBytes = 0;
    mTransferBytesTotal = 0;

    mState = TransferModel::Connecting;
    mError.clear();

    mProgress = 0;

    mBuffer.clear();
    mBufferSize = 0;

    // For simplicity, signal that all roles have changed
    emit dataChanged();
}

void Transfer::finish(TransferModel::State state)
{
    // Change the state and emit the signal indicating the change
    mState = state;
    emit dataChanged({TransferModel::StateRole});

    // Close the socket and any open file
    mSocket->abort();
    mFile.close();
}
