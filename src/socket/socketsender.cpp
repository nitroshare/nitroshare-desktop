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
#include <QVariantMap>

#include "../util/settings.h"
#include "socketsender.h"

SocketSender::SocketSender(const Device *device, BundlePointer bundle)
    : mAddress(device->address()),
      mPort(device->port()),
      mBundle(bundle)
{
    // As soon as the connection succeeds, write the first packet
    connect(this, &SocketSender::connected, [this]() {
        writeNextPacket();
    });
}

void SocketSender::start()
{
    // Reset everything
    mState = WritingTransferHeader;
    mIterator = mBundle->constBegin();
    mTransferBytes = 0;

    if(mFile.isOpen()) {
        mFile.close();
    }
    mFileRemainingBytes = 0;

    mBuffer.resize(Settings::get<int>(Settings::TransferBuffer));

    // Attempt connection to receiver
    connectToHost(mAddress, mPort);
}

void SocketSender::processPacket(const QByteArray &data)
{
    // This is not currently used for sending transfers
}

void SocketSender::writeNextPacket()
{
    switch(mState) {
    case WritingTransferHeader:
        writeTransferHeader();
        break;
    case WritingFileHeader:
        writeFileHeader();
        break;
    case WritingFile:
        writeFile();
        break;
    }
}

void SocketSender::writeTransferHeader()
{
    // Write the transfer header
    QJsonObject object = QJsonObject::fromVariantMap({
        { "protocol", 1 },
        { "name", Settings::get<QString>(Settings::DeviceName) },
        { "size", mBundle->totalSize() },
        { "count", mBundle->count() }
    });
    writePacket(QJsonDocument(object).toJson(QJsonDocument::Compact));

    // Move on to writing the files
    mState = WritingFileHeader;
}

void SocketSender::writeFileHeader()
{
    // Open the file and obtain its size
    mFile.setFileName(mIterator->absoluteFilename());

    if(mFile.open(QIODevice::ReadOnly)) {
        mFileRemainingBytes = mFile.size();

        // Write the file header
        QJsonObject object = QJsonObject::fromVariantMap({
            { "name", mIterator->filename() },
            { "size", mFileRemainingBytes }
        });
        writePacket(QJsonDocument(object).toJson(QJsonDocument::Compact));

        // Switch the state to file writing
        mState = WritingFile;
    } else {
        emit transferError(tr("Unable to read %1").arg(mIterator->absoluteFilename()));
    }
}

void SocketSender::writeFile()
{
    // Read the next chunk of data from the file
    qint64 bytesRead = mFile.read(mBuffer.data(), qMin(mFileRemainingBytes, static_cast<qint64>(mBuffer.size())));

    // Ensure that a valid number of bytes were read
    if(bytesRead <= 0) {
        emit transferError(tr("Unable to read from %1").arg(mFile.fileName()));
        return;
    }

    // Write a packet containing the data that was just read
    writePacket(mBuffer.left(bytesRead));

    // Update the number of bytes remaining in the file and the total transferred
    mFileRemainingBytes -= bytesRead;
    mTransferBytes += bytesRead;

    // TODO: emit progress

    // Check to see if we wrote the entire file contents
    if(!mFileRemainingBytes) {
        // Check to see if there are any files left
        if(mIterator == mBundle->constEnd()) {
            // None left, emit the success signal
            emit success();
        } else {
            // Otherwise, move on to the next file
            mIterator++;
            mState = WritingFileHeader;
        }
    }
}
