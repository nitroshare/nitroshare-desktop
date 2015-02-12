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

#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>

#include "../filesystem/fileinfo.h"
#include "../util/settings.h"
#include "socketreceiver.h"

SocketReceiver::SocketReceiver(qintptr socketDescriptor)
    : mRoot(Settings::get<QString>(Settings::TransferDirectory))
{
    setSocketDescriptor(socketDescriptor);
}

void SocketReceiver::initialize()
{
    // Socket is already connected at this point, do nothing
}

void SocketReceiver::processPacket(const QByteArray &data)
{
    // Depending on the state of the transfer, process the packet accordingly
    switch(mState){
    case TransferHeader:
        processTransferHeader(data);
        break;
    case FileHeader:
        processFileHeader(data);
        break;
    case FileData:
        processFileData(data);
        break;
    }
}

void SocketReceiver::writeNextPacket()
{
    // This should never be invoked since no data is written
}

void SocketReceiver::processTransferHeader(const QByteArray &data)
{
    QJsonDocument document = QJsonDocument::fromJson(data);

    // Ensure that the transfer header was readable
    if(document.isEmpty()) {
        throw tr("Unable to read transfer header");
    }

    QJsonObject object = document.object();

    // TODO: do more error checking here
    emit deviceName(object.value("name").toString());
    mTransferBytesTotal = object.value("size").toString().toLongLong();
    mTransferFilesRemaining = object.value("count").toString().toLongLong();

    // The next packet will be the first file header
    mState = FileHeader;
}

void SocketReceiver::processFileHeader(const QByteArray &data)
{
    QJsonDocument document = QJsonDocument::fromJson(data);

    // Ensure that the file header was readable
    if(document.isEmpty()) {
        throw tr("Unable to read file header");
    }

    QJsonObject object = document.object();

    // Obtain the filename
    QString filename = object.value("name").toString();
    FileInfo info(mRoot, filename);

    // Ensure that the path exists
    QDir path(QFileInfo(info.absoluteFilename()).path());
    if(!path.exists()) {
        if(!path.mkpath(".")) {
            throw tr("Unable to create %1").arg(path.absolutePath());
        }
    }

    // TODO: do more error checking here
    mFile.setFileName(info.absoluteFilename());
    mFileBytesRemaining = object.value("size").toString().toLongLong();

    // Abort if the file can't be opened
    if(!mFile.open(QIODevice::WriteOnly)) {
        throw tr("Unable to open %1").arg(mFile.fileName());
    }

    // The next packet will be the first chunk from the file
    mState = FileData;
}

void SocketReceiver::processFileData(const QByteArray &data)
{
    // Write the data to the file
    mFile.write(data);

    // Update the number of bytes remaining for the file and the total transferred
    mFileBytesRemaining -= data.size();
    mTransferBytes += data.size();

    // Provide a progress update
    emitProgress();

    // If there are no more bytes to write to the file, move on the
    // next file or indicate that the transfer has completed
    if(mFileBytesRemaining <= 0) {
        mFile.close();
        mTransferFilesRemaining -= 1;

        if(!mTransferFilesRemaining) {
            emit success();
        } else {
            mState = FileHeader;
        }
    }
}
