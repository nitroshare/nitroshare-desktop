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

#include "../util/jsonvalidator.h"
#include "../util/settings.h"
#include "transferreceiver.h"

TransferReceiver::TransferReceiver(qintptr socketDescriptor)
    : Transfer(TransferModel::Receive),
      mRoot(Settings::get<QString>(Settings::TransferDirectory))
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
    case FileHeader:
        processFileHeader(data);
        break;
    case FileData:
        processFileData(data);
        break;
    case Finished:
        break;
    }
}

void TransferReceiver::writeNextPacket()
{
    // This should never be invoked since no data is written
}

void TransferReceiver::processTransferHeader(const QByteArray &data)
{
    QJsonDocument document = QJsonDocument::fromJson(data);
    QJsonObject object;

    if(JsonValidator::isObject(document, object) &&
            JsonValidator::objectContains(object, "name", mDeviceName) &&
            JsonValidator::objectContains(object, "size", mTransferBytesTotal) &&
            JsonValidator::objectContains(object, "count", mTransferFilesRemaining)) {

        emit dataChanged({TransferModel::DeviceNameRole});

        // The next packet will be the first file header
        mProtocolState = FileHeader;
    } else {

        abortWithError(tr("Unable to read transfer header"));
    }
}

void TransferReceiver::processFileHeader(const QByteArray &data)
{
    QJsonDocument document = QJsonDocument::fromJson(data);
    QJsonObject object;
    QString name;

    if(JsonValidator::isObject(document, object) &&
            JsonValidator::objectContains(object, "name", name) &&
            JsonValidator::objectContains(object, "size", mFileBytesRemaining)) {

        FileInfo info(mRoot, name);

        // Ensure that the path exists
        QDir path(QFileInfo(info.absoluteFilename()).path());
        if(!path.exists()) {
            if(!path.mkpath(".")) {
                abortWithError(tr("Unable to create %1").arg(path.absolutePath()));
                return;
            }
        }

        // Abort if the file can't be opened
        mFile.setFileName(info.absoluteFilename());
        if(!mFile.open(QIODevice::WriteOnly)) {
            abortWithError(tr("Unable to open %1").arg(mFile.fileName()));
            return;
        }

        // The next packet will be the first chunk from the file
        mProtocolState = FileData;
    } else {

        abortWithError(tr("Unable to read file header"));
    }
}

void TransferReceiver::processFileData(const QByteArray &data)
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
        mTransferFilesRemaining -= 1;

        if(!mTransferFilesRemaining) {
            finish();
        } else {
            mProtocolState = FileHeader;
        }
    }
}
