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

#include "../bundle/bundle_p.h"
#include "../util/settings.h"
#include "transfersender.h"

TransferSender::TransferSender(const QString &deviceName, const QHostAddress &address, quint16 port, const Bundle *bundle)
    : Transfer(TransferModel::Send),
      mAddress(address),
      mPort(port),
      mBundle(bundle)
{
    mDeviceName = deviceName;

    // As soon as the connection completes, begin writing packets
    connect(&mSocket, &QTcpSocket::connected, [this]() {
        writeNextPacket();
    });
}

TransferSender::~TransferSender()
{
    delete mBundle;
}

void TransferSender::start()
{
    mTransferBytesTotal = mBundle->totalSize();
    mIterator = mBundle->d->items.constBegin();

    if(mFile.isOpen()) {
        mFile.close();
    }

    mFileBuffer.resize(Settings::get<int>(Settings::TransferBuffer));

    // Attempt to connect to the other device
    mSocket.connectToHost(mAddress, mPort);
}

void TransferSender::processPacket(const QByteArray &)
{
    // Any data received here is an error and should be treated as such
    abortWithError(tr("Unexpected data received"));
}

void TransferSender::writeNextPacket()
{
    // This is invoked when there is no data waiting to be written
    // Depending on the current state, send the next packet
    switch(mProtocolState) {
    case TransferHeader:
        writeTransferHeader();
        break;
    case ItemHeader:
        writeItemHeader();
        break;
    case ItemData:
        writeItemData();
        break;
    case Finished:
        finish();
    }
}

void TransferSender::writeTransferHeader()
{
    // Due to poor translation between 64-bit integers in C++ and JSON,
    // it is necessary to send integers as strings
    writePacket({
        { "protocol", "1" },
        { "name", Settings::get<QString>(Settings::DeviceName) },
        { "size", QString::number(mTransferBytesTotal) },
        { "count", QString::number(mBundle->count()) }
    });

    // The next packet will be the first item header
    mProtocolState = ItemHeader;
}

void TransferSender::writeItemHeader()
{
    // Build the header describing the item
    QVariantMap header = {
        { "name", mIterator->relativeFilename() },
        { "directory", mIterator->isDir() },
        { "created", mIterator->created() },
        { "last_modified", mIterator->lastModified() },
        { "last_read", mIterator->lastRead() }
    };

    // If the item is a directory, write the header and move to the next item
    // Otherwise open the file and prepare for reading data
    if(mIterator->isDir()) {
        writePacket(header);
        nextItem();
    } else {

        // Set the filename and attempt to open the file
        mFile.setFileName(mIterator->absoluteFilename());
        if(!mFile.open(QIODevice::ReadOnly)) {
            abortWithError(tr("Unable to open %1").arg(mIterator->absoluteFilename()));
            return;
        }

        // Obtain the file size, add it to the header, and send it
        mFileBytesRemaining = mFile.size();
        header.insert("size", QString::number(mFileBytesRemaining));
        writePacket(header);

        // If the file is non-empty, switch states
        // Otherwise close the file and move to the next item
        if(mFileBytesRemaining) {
            mProtocolState = ItemData;
        } else {
            mFile.close();
            nextItem();
        }
    }
}

void TransferSender::writeItemData()
{
    // Read the next chunk of data from the file - no more than either the size
    // of the buffer or the amount of data remaining in the file
    qint64 bytesRead = mFile.read(mFileBuffer.data(),
            qMin(mFileBytesRemaining, static_cast<qint64>(mFileBuffer.size())));

    // Ensure that a valid number of bytes were read
    if(bytesRead <= 0 && mFileBytesRemaining) {
         abortWithError(tr("Unable to read from %1").arg(mFile.fileName()));
         return;
    }

    // Write a packet containing the data that was just read
    writePacket(mFileBuffer.left(bytesRead));

    // Update the number of bytes remaining in the file and the total transferred
    mFileBytesRemaining -= bytesRead;
    mTransferBytes += bytesRead;

    // Provide a progress update
    calculateProgress();

    // If there are no bytes remaining in the file close it and move on
    if(!mFileBytesRemaining) {
        mFile.close();
        nextItem();
    }
}

void TransferSender::nextItem()
{
    // Move to the next item
    ++mIterator;

    // Check to see if the "next item" actually exists or if we're done
    if(mIterator == mBundle->d->items.constEnd()) {
        mProtocolState = Finished;
    } else {
        mProtocolState = ItemHeader;
    }
}
