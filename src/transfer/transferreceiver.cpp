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

#include <QJsonDocument>

#include "../util/json.h"
#include "../util/settings.h"
#include "transferreceiver.h"

TransferReceiver::TransferReceiver(qintptr socketDescriptor)
    : Transfer(TransferModel::Receive),
      mRoot(Settings::get(Settings::TransferDirectory).toString())
{
    mSocket.setSocketDescriptor(socketDescriptor);
}

void TransferReceiver::start()
{
    // The socket is already connected at this point
}

void TransferReceiver::processPacket(const QByteArray &data)
{
    // Depending on the state of the transfer, process the packet accordingly
    switch(mProtocolState){
    case TransferHeader:
        processTransferHeader(data);
        break;
    case ItemHeader:
        processItemHeader(data);
        break;
    case ItemData:
        processItemData(data);
        break;
    case Finished:
        break;
    }
}

void TransferReceiver::writeNextPacket()
{
    // The only time data is written to the socket is after
    // the transfer has completed OR an error condition occurs
    if(mError.isNull()) {
        finish();
    } else {
        abortWithError(mError);
    }
}

void TransferReceiver::processTransferHeader(const QByteArray &data)
{
    QJsonDocument document = QJsonDocument::fromJson(data);
    QJsonObject object;

    if(Json::isObject(document, object) &&
            Json::objectContains(object, "name", mDeviceName) &&
            Json::objectContains(object, "size", mTransferBytesTotal) &&
            Json::objectContains(object, "count", mTransferItemsRemaining)) {

        emit dataChanged({TransferModel::DeviceNameRole});

        // The next packet will be the first file header
        mProtocolState = ItemHeader;

    } else {
        abortWithError(tr("Unable to read transfer header"));
    }
}

void TransferReceiver::processItemHeader(const QByteArray &data)
{
    QJsonDocument document = QJsonDocument::fromJson(data);
    QJsonObject object;
    QString name;
    bool directory;
    qint64 created;
    qint64 lastModified;
    qint64 lastRead;

    // Read the contents of the header
    if(Json::isObject(document, object) &&
            Json::objectContains(object, "name", name) &&
            Json::objectContains(object, "directory", directory) &&
            Json::objectContains(object, "created", created) &&
            Json::objectContains(object, "last_modified", lastModified) &&
            Json::objectContains(object, "last_read", lastRead)) {

        // TODO: created, lastModified, and lastRead are unused

        // Determine the absolute filename of the item
        QString filename = mRoot.absoluteFilePath(name);

        // If the item is a directory, attempt to create it
        // Otherwise, open the file for writing since the directory should exist
        if(directory) {

            if(!QDir(filename).mkpath(".")) {
                sendError(tr("Unable to create %1").arg(filename));
                return;
            }

            // Move to the next item
            nextItem();

        } else {

            // Ensure that the size was included
            if(!Json::objectContains(object, "size", mFileBytesRemaining)) {
                sendError(tr("File size is missing from header"));
                return;
            }

            // Abort if the file can't be opened
            mFile.setFileName(filename);
            if(!mFile.open(QIODevice::WriteOnly)) {
                sendError(tr("Unable to open %1").arg(filename));
                return;
            }

            // If the file is non-empty, switch states
            // Otherwise close the file and move to the next item
            if(mFileBytesRemaining) {
                mProtocolState = ItemData;
            } else {
                mFile.close();
                nextItem();
            }
        }

    } else {
        sendError(tr("Unable to read file header"));
    }
}

void TransferReceiver::processItemData(const QByteArray &data)
{
    // Write the data to the file
    mFile.write(data);

    // Update the number of bytes remaining for the file and the total transferred
    mFileBytesRemaining -= data.size();
    mTransferBytes += data.size();

    // Provide a progress update
    calculateProgress();

    // If there are no more bytes to write to the file, move on the
    // next file or indicate that the transfer has completed
    if(mFileBytesRemaining <= 0) {
        mFile.close();
        nextItem();
    }
}

void TransferReceiver::sendError(const QString &message)
{
    // Send a packet with the error message
    QVariantMap packet = {
        { "error", message }
    };
    writePacket(packet);

    // Store the error message and mark the transfer as finished
    // This will cause all remaining packets to be discarded
    mError = message;
    mProtocolState = Finished;
}

void TransferReceiver::nextItem()
{
    // Decrement the number of items remaining
    mTransferItemsRemaining -= 1;

    // Check to see if there are any more items remaining
    if(!mTransferItemsRemaining) {

        // Write the "success" packet (currently empty)
        writePacket(QVariantMap());

        mProtocolState = Finished;
    } else {
        mProtocolState = ItemHeader;
    }
}
