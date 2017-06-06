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

#include <QRegularExpression>
#include <QRegularExpressionMatch>

#include "../settings/settings.h"
#include "../util/json.h"
#include "transferreceiver.h"

TransferReceiver::TransferReceiver(QSslConfiguration *configuration, qintptr socketDescriptor)
    : Transfer(configuration, TransferModel::Receive),
      mRoot(Settings::instance()->get(Settings::Key::TransferDirectory).toString()),
      mOverwrite(Settings::instance()->get(Settings::Key::BehaviorOverwrite).toBool())
{
    mSocket->setSocketDescriptor(socketDescriptor);
    mDeviceName = tr("[Unknown]");

    // Trigger server-side TLS handshake
    onConnected();
}

void TransferReceiver::startConnect()
{
    // The socket is already connected at this point
}

void TransferReceiver::startTransfer()
{
    // The client starts the transfer
}

void TransferReceiver::processJsonPacket(const QJsonObject &object)
{
    switch (mProtocolState) {
    case ProtocolState::TransferHeader:
        processTransferHeader(object);
        break;
    case ProtocolState::TransferItems:
        processItemHeader(object);
        break;
    default:
        writeErrorPacket(tr("Unexpected JSON packet received"));
    }
}

void TransferReceiver::processBinaryPacket(const QByteArray &data)
{
    // Binary packets can only be received after the transfer header
    // and after a file header - for sanity, we check the amount of
    // data we're expecting to receive from the amount actually received

    if (mProtocolState == ProtocolState::TransferItems) {

        // The file will only be open if an item header was received prior
        // This error is also triggered if more data for the previous file arrives
        if (!mFile.isOpen()) {
            writeErrorPacket(tr("Binary packet received before item header"));
            return;
        }

        // Ensure that the size of the packet does not exceed the amount expected
        if (data.size() > mFileBytesRemaining) {
            writeErrorPacket(tr("Binary packet exceeds declared size"));
            return;
        }

        // Write the data to the file
        mFile.write(data);

        // Update the number of bytes remaining for the file and the total transferred
        mFileBytesRemaining -= data.size();
        mTransferBytes += data.size();

        // Provide a progress update
        updateProgress();

        // If all of the file has been received, move on to the next item
        if (!mFileBytesRemaining) {
            mFile.close();
            nextItem();
        }

    } else {
        writeErrorPacket(tr("Unexpected binary packet received"));
    }
}

void TransferReceiver::writeNextPacket()
{
    // No data is written after the success or error packet
}

void TransferReceiver::processTransferHeader(const QJsonObject &object)
{
    QString deviceName;

    if (Json::objectContains(object, "name", deviceName) &&
            Json::objectContains(object, "size", mTransferBytesTotal) &&
            Json::objectContains(object, "count", mTransferItemsRemaining)) {

        // Indicate that the device name has changed
        mDeviceName = deviceName;
        emit dataChanged({TransferModel::DeviceNameRole});

        // The next packet will be the first file header
        mProtocolState = ProtocolState::TransferItems;

    } else {
        writeErrorPacket(tr("Unable to read transfer header"));
    }
}

void TransferReceiver::processItemHeader(const QJsonObject &object)
{
    // Before doing anything, ensure that the entire contents of
    // the previous file were received and that the file was closed
    if (mFile.isOpen()) {
        writeErrorPacket(tr("Item header received before previous file contents"));
        return;
    }

    QString name;
    bool directory;
    qint64 created;
    qint64 lastModified;
    qint64 lastRead;

    if (Json::objectContains(object, "name", name) &&
            Json::objectContains(object, "directory", directory) &&
            Json::objectContains(object, "created", created) &&
            Json::objectContains(object, "last_modified", lastModified) &&
            Json::objectContains(object, "last_read", lastRead)) {

        // TODO: created, lastModified, and lastRead are currently unused

        // Determine the absolute filename of the item
        QString filename = mRoot.absoluteFilePath(name);

        // If the item is a directory, attempt to create it
        // Otherwise, it's a file - open it for writing
        if (directory) {

            // Ensure the directory exists
            if (!QDir(filename).mkpath(".")) {
                writeErrorPacket(tr("Unable to create %1").arg(filename));
                return;
            }

            // Move to the next item
            nextItem();

        } else {

            // Ensure that the size was included
            if (!Json::objectContains(object, "size", mFileBytesRemaining)) {
                writeErrorPacket(tr("File size is missing from item header"));
                return;
            }

            // Ensure the parent directory exists
            QDir parent(filename);
            if (!parent.cdUp() || !parent.mkpath(".")) {
                writeErrorPacket(tr("Unable to create directory for %1").arg(filename));
                return;
            }

            // Abort if the file can't be opened
            mFile.setFileName(filename);
            if (!openFile()) {
                writeErrorPacket(tr("Unable to open %1").arg(filename));
                return;
            }

            // If the file is empty, we'll never receive its contents (surprise!)
            // Therefore, we must immediately close the file and move to the next item
            if (!mFileBytesRemaining) {
                mFile.close();
                nextItem();
            }
        }

    } else {
        writeErrorPacket(tr("Unable to read file header"));
    }
}

void TransferReceiver::nextItem()
{
    // Decrement the number of items remaining
    mTransferItemsRemaining -= 1;

    // If no more items remain, then write the success packet
    if (!mTransferItemsRemaining) {
        writeSuccessPacket();
    }
}

bool TransferReceiver::openFile()
{
    // NOTE: there is a known race-condition in the code below - it
    // is possible that a file could be created between checking for
    // the file and attempting to open it. However, this is incredibly
    // hard to work around and unlikely to be a problem in most normal
    // circumstances.

    // If overwrite is enabled OR the file doesn't exist, try
    // to open the file with the original filename
    if (mOverwrite || !mFile.exists()) {
        return mFile.open(QIODevice::WriteOnly);
    }

    // Break the filename into its components
    QRegularExpression re("^(.*?)((?:\\.tar)?\\.[^\\/\\\\]*)?$");
    QRegularExpressionMatch match = re.match(mFile.fileName());
    if (!match.hasMatch()) {
        return false;
    }

    // Try consecutive numbers
    int num = 2;
    while (mFile.exists()) {
        mFile.setFileName(QString("%1-%2%3")
                .arg(match.captured(1))
                .arg(num++)
                .arg(match.captured(2)));
    }

    // One was found that doesn't exist - try opening it
    return mFile.open(QIODevice::WriteOnly);
}
