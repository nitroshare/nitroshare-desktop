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
#include <QJsonObject>

#include "socketreceiver.h"

SocketReceiver::SocketReceiver(qintptr socketDescriptor)
    : mState(WaitingForTransferHeader)
{
    setSocketDescriptor(socketDescriptor);
}

void SocketReceiver::processPacket(const QByteArray &data)
{
    // Depending on the state of the transfer, process the packet accordingly
    switch(mState){
    case WaitingForTransferHeader:
        processTransferHeader(data);
        break;
    case WaitingForFileHeader:
        processFileHeader(data);
        break;
    case WaitingForFile:
        processFile(data);
        break;
    }
}

void SocketReceiver::writeNextPacket()
{
    // This is not currently used for receiving transfers
}

void SocketReceiver::processTransferHeader(const QByteArray &data)
{
    QJsonDocument document = QJsonDocument::fromJson(data);
    if(!document.isEmpty()) {
        QJsonObject object = document.object();

        // TODO: do more error checking here
        emit deviceName(object.value("name").toString());
        mTransferTotalBytes = object.value("size").toString().toLongLong();
        mTransferRemainingFiles = object.value("count").toString().toLongLong();

        // Move on to waiting for the first file header
        mState = WaitingForFileHeader;
    } else {
        emit transferError(tr("Unable to read transfer header"));
    }
}

void SocketReceiver::processFileHeader(const QByteArray &data)
{
    QJsonDocument document = QJsonDocument::fromJson(data);
    if(!document.isEmpty()) {
        QJsonObject object = document.object();

        // TODO: do more error checking here
        mFile.setFileName(object.value("name").toString());
        mFileRemainingBytes = object.value("size").toString().toLongLong();

        // If the file can be opened, switch states
        if(!mFile.open(QIODevice::WriteOnly)) {
            mState = WaitingForFile;
        } else {
            emit transferError(tr("Unable to write %1").arg(mFile.fileName()));
        }
    } else {
        emit transferError(tr("Unable to read file header"));
    }
}

void SocketReceiver::processFile(const QByteArray &data)
{
    // Write the data to the file
    mFile.write(data);
    mFileRemainingBytes -= data.size();
    mTransferBytes += data.size();

    // TODO: Update progress

    // Check to see if that was the end of the file
    if(mFileRemainingBytes <= 0) {
        // If so, close the file and begin waiting for the next header
        mFile.close();
        mState = WaitingForFileHeader;
        mTransferRemainingFiles -= 1;

        // Check to see if that was the end of the transfer
        if(mTransferRemainingFiles) {
            // If so, we're done!
            emit success();
        }
    }
}
