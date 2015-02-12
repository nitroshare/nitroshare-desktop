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

void SocketSender::initialize()
{
    mTransferBytesTotal = mBundle->totalSize();

    mState = WritingTransferHeader;
    mIterator = mBundle->constBegin();

    if(mFile.isOpen()) {
        mFile.close();
    }

    mFileBuffer.resize(Settings::get<int>(Settings::TransferBuffer));

    // If this succeeds, connected will be emitted and writeNextPacket called
    connectToHost(mAddress, mPort);
}

void SocketSender::processPacket(const QByteArray &)
{
    // Any data received here is completely unexpected
    emit transferError(tr("Unexpected data received"));
}

void SocketSender::writeNextPacket()
{
    // This is invoked when there is no data waiting to be written
    // Depending on the current state, send the next packet
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
    case Finished:
        emit success();
    }
}

void SocketSender::writeTransferHeader()
{
    // Due to poor translation between 64-bit integers in C++ and JSON,
    // it is necessary to send these integers as strings
    QJsonObject object = QJsonObject::fromVariantMap({
        { "protocol", 1 },
        { "name", Settings::get<QString>(Settings::DeviceName) },
        { "size", QString::number(mTransferBytesTotal) },
        { "count", QString::number(mBundle->count()) }
    });

    // Write the packet containing the header and switch states
    writePacket(QJsonDocument(object).toJson(QJsonDocument::Compact));
    mState = WritingFileHeader;
}

void SocketSender::writeFileHeader()
{
    mFile.setFileName(mIterator->absoluteFilename());
    if(mFile.open(QIODevice::ReadOnly)) {
        mFileBytesRemaining = mFile.size();

        QJsonObject object = QJsonObject::fromVariantMap({
            { "name", mIterator->filename() },
            { "size", QString::number(mFileBytesRemaining) }
        });

        // Write the packet containing the header and switch states
        writePacket(QJsonDocument(object).toJson(QJsonDocument::Compact));
        mState = WritingFile;
    } else {
        emit transferError(tr("Unable to open %1").arg(mIterator->absoluteFilename()));
    }
}

void SocketSender::writeFile()
{
    // Read the next chunk of data from the file - no more than either the size
    // of the buffer or the amount of data remaining in the file
    qint64 bytesRead = mFile.read(mFileBuffer.data(),
            qMin(mFileBytesRemaining, static_cast<qint64>(mFileBuffer.size())));

    // Ensure that a valid number of bytes were read
    if(bytesRead <= 0) {
        emit transferError(tr("Unable to read from %1").arg(mFile.fileName()));
        return;
    }

    // Write a packet containing the data that was just read
    writePacket(mFileBuffer.left(bytesRead));

    // Update the number of bytes remaining in the file and the total transferred
    mFileBytesRemaining -= bytesRead;
    mTransferBytes += bytesRead;

    // Provide a progress update
    emitProgress();

    // If there are no bytes remaining in the file, move on to the next file
    // or indicate that the transfer has completed (emit success after write completes)
    if(!mFileBytesRemaining) {
        mFile.close();

        if(mIterator == mBundle->constEnd()) {
            mState = Finished;
        } else {
            ++mIterator;
            mState = WritingFileHeader;
        }
    }
}
