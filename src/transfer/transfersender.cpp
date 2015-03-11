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

#include "../bundle/bundle_p.h"
#include "../settings/settings.h"
#include "../util/json.h"
#include "transfersender.h"

TransferSender::TransferSender(const QString &deviceName, const QHostAddress &address, quint16 port, const Bundle *bundle)
    : Transfer(TransferModel::Send),
      mAddress(address),
      mPort(port),
      mBundle(bundle),
      mBufferSize(Settings::instance()->get(Settings::Key::TransferBuffer).toInt())
{
    mDeviceName = deviceName;
}

TransferSender::~TransferSender()
{
    delete mBundle;
}

void TransferSender::start()
{
    mTransferBytesTotal = mBundle->totalSize();
    mCurrentItem = mBundle->d->items.constBegin();

    // Attempt to connect to the other device
    mSocket.connectToHost(mAddress, mPort);
}

void TransferSender::processJsonPacket(const QJsonObject &)
{
    // JSON packets should never be received by the sender
    writeErrorPacket(tr("Unexpected JSON packet received"));
}

void TransferSender::processBinaryPacket(const QByteArray &)
{
    // Binary packets should never be received by the sender
    writeErrorPacket(tr("Unexpected binary packet received"));
}

void TransferSender::writeNextPacket()
{
    // Write the next packet according to the current state
    switch(mProtocolState) {
    case ProtocolState::TransferHeader:
        writeTransferHeader();
        break;
    case ProtocolState::TransferItems:
        if(mFile.isOpen()) {
            writeItemData();
        } else {
            writeItemHeader();
        }
        break;
    case ProtocolState::Acknowledgement:
    case ProtocolState::Finished:
        break;
    }
}

void TransferSender::writeTransferHeader()
{
    // Due to poor translation between 64-bit integers in C++ and JSON,
    // it is necessary to send integers as strings
    writeJsonPacket(QJsonObject::fromVariantMap({
        { "name", Settings::instance()->get(Settings::Key::DeviceName).toString() },
        { "size", QString::number(mTransferBytesTotal) },
        { "count", QString::number(mBundle->count()) }
    }));

    mProtocolState = ProtocolState::TransferItems;
}

void TransferSender::writeItemHeader()
{
    // Build the header describing the item
    QJsonObject header = QJsonObject::fromVariantMap({
        { "name", mCurrentItem->relativeFilename() },
        { "directory", mCurrentItem->isDir() },
        { "created", QString::number(mCurrentItem->created()) },
        { "last_modified", QString::number(mCurrentItem->lastModified()) },
        { "last_read", QString::number(mCurrentItem->lastRead()) }
    });

    // If the item is a directory, write the header and move to the next item
    // Otherwise open the file and prepare for reading data
    if(mCurrentItem->isDir()) {
        writeJsonPacket(header);
        nextItem();
    } else {

        // Set the filename and attempt to open the file
        mFile.setFileName(mCurrentItem->absoluteFilename());
        if(!mFile.open(QIODevice::ReadOnly)) {
            writeErrorPacket(tr("Unable to open %1").arg(mCurrentItem->absoluteFilename()));
            return;
        }

        // Obtain the file size, add it to the header, and send it
        mFileBytesRemaining = mFile.size();
        header.insert("size", QString::number(mFileBytesRemaining));
        writeJsonPacket(header);

        // If the file is empty, there's no need to send its contents
        if(!mFileBytesRemaining) {
            mFile.close();
            nextItem();
        }
    }
}

void TransferSender::writeItemData()
{
    // Read the next chunk of data from the file - no more than either the size
    // of the buffer or the amount of data remaining in the file
    QByteArray buffer;
    buffer.resize(mBufferSize);

    qint64 bytesRead = mFile.read(buffer.data(), qMin(mFileBytesRemaining, static_cast<qint64>(mBufferSize)));

    // Ensure that a valid number of bytes were read
    if(bytesRead < 0 && mFileBytesRemaining) {
         writeErrorPacket(tr("Unable to read from %1").arg(mFile.fileName()));
         return;
    }

    // Write a packet containing the data that was just read
    writeBinaryPacket(buffer.left(bytesRead));

    // Update the number of bytes remaining in the file and the total transferred
    mFileBytesRemaining -= bytesRead;
    mTransferBytes += bytesRead;

    // Provide a progress update
    updateProgress();

    // If there are no bytes remaining in the file close it and move on
    if(!mFileBytesRemaining) {
        mFile.close();
        nextItem();
    }
}

void TransferSender::nextItem()
{
    // Move to the next item
    ++mCurrentItem;

    // Check to see if the "next item" actually exists or if we're done
    if(mCurrentItem == mBundle->d->items.constEnd()) {
        mProtocolState = ProtocolState::Acknowledgement;
    }
}
