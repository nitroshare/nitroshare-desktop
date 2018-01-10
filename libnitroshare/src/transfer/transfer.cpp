/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Nathan Osman
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
 */

#include <cstring>

#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QtEndian>

#include <nitroshare/application.h>
#include <nitroshare/bundle.h>
#include <nitroshare/device.h>
#include <nitroshare/handler.h>
#include <nitroshare/handlerregistry.h>
#include <nitroshare/item.h>
#include <nitroshare/jsonutil.h>
#include <nitroshare/logger.h>
#include <nitroshare/message.h>
#include <nitroshare/packet.h>
#include <nitroshare/transfer.h>
#include <nitroshare/transfermodel.h>
#include <nitroshare/transport.h>
#include <nitroshare/transportserverregistry.h>

#include "transfer_p.h"

const QString MessageTag = "transfer";

// Interval for calculating transfer speed
const qint64 SpeedInterval = 1000;

TransferPrivate::TransferPrivate(Transfer *transfer,
                                 Application *application,
                                 Device *device,
                                 Transport *transport,
                                 Bundle *bundle)
    : QObject(transfer),
      q(transfer),
      mApplication(application),
      mTransport(transport),
      mBundle(bundle),
      mProtocolState(TransferHeader),
      mDirection(device ? Transfer::Send : Transfer::Receive),
      mState(device ? Transfer::Connecting : Transfer::InProgress),
      mProgress(0),
      mDeviceName(device ? device->name() : tr("[unknown]")),
      mItemIndex(0),
      mItemCount(bundle ? bundle->rowCount() : 0),
      mBytesTransferred(0),
      mBytesTotal(bundle ? bundle->totalSize() : 0),
      mCurrentItem(nullptr),
      mCurrentItemBytesTransferred(0),
      mCurrentItemBytesTotal(0),
      mSpeed(0),
      mLastInterval(QDateTime::currentMSecsSinceEpoch()),
      mLastIntervalBytesTransferred(0)
{
    connect(&mSpeedTimer, &QTimer::timeout, this, &TransferPrivate::onTimeout);

    if (mDirection == Transfer::Send) {

        // Use the device to attempt to create a transport
        mTransport = application->transportServerRegistry()->createTransport(device);
        if (!mTransport) {
            setError(tr("unable to create \"%1\" transport").arg(device->transportName()));
            return;
        }
        connect(mTransport, &Transport::connected, this, &TransferPrivate::onConnected);

        // Ensure the bundle is freed when the transfer is destroyed
        mBundle->setParent(this);
    } else {
        mSpeedTimer.start(SpeedInterval);
    }

    // Transport should always be valid at this point - ensure it is freed
    mTransport->setParent(this);

    connect(mTransport, &Transport::packetReceived, this, &TransferPrivate::onPacketReceived);
    connect(mTransport, &Transport::packetSent, this, &TransferPrivate::onPacketSent);
    connect(mTransport, &Transport::error, this, &TransferPrivate::onError);
}

void TransferPrivate::sendTransferHeader()
{
    QJsonObject object{
        { "name", mApplication->deviceName() },
        { "count", QString::number(mBundle->rowCount(QModelIndex())) },
        { "size", QString::number(mBundle->totalSize()) }
    };

    Packet packet(Packet::Json, QJsonDocument(object).toJson());
    mTransport->sendPacket(&packet);

    // The next packet will be an item header
    mProtocolState = ItemHeader;
}

void TransferPrivate::sendItemHeader()
{
    // Grab the next item and attempt to open it
    mCurrentItem = mBundle->index(mItemIndex, 0).data(Qt::UserRole).value<Item*>();
    if (!mCurrentItem->open(Item::Read)) {
        setError(tr("unable to open \"%1\" for reading").arg(mCurrentItem->name()), true);
        return;
    }

    // Reset transfer stats
    mCurrentItemBytesTransferred = 0;
    mCurrentItemBytesTotal = mCurrentItem->size();

    // Build a JSON object with all of the properties
    QJsonObject object = JsonUtil::objectToJson(mCurrentItem);

    // Send the item header
    Packet packet(Packet::Json, QJsonDocument(object).toJson());
    mTransport->sendPacket(&packet);

    // If the item has a size, switch states; otherwise send the next item
    if (mCurrentItemBytesTotal) {
        mProtocolState = ItemContent;
    } else {
        sendNext();
    }
}

void TransferPrivate::sendItemContent()
{
    QByteArray data = mCurrentItem->read();
    Packet packet(Packet::Binary, data);
    mTransport->sendPacket(&packet);

    // Increment the number of bytes written to the socket
    mBytesTransferred += data.length();
    mCurrentItemBytesTransferred += data.length();
    mLastIntervalBytesTransferred += data.length();

    updateProgress();

    // If the item completed, send the next one
    if (mCurrentItemBytesTransferred >= mCurrentItemBytesTotal) {
        sendNext();
    }
}

void TransferPrivate::sendNext()
{
    // Close the current item and increment the index
    mCurrentItem->close();
    ++mItemIndex;

    // If all items have been sent, move to the finished state and wait for
    // the success packet; otherwise, prepare to send the next item
    if (mItemIndex == mItemCount) {
        mProtocolState = Finished;
    } else {
        mProtocolState = ItemHeader;
    }
}

void TransferPrivate::processTransferHeader(Packet *packet)
{
    QJsonParseError error;
    QJsonObject object = QJsonDocument::fromJson(packet->content(), &error).object();
    if (error.error != QJsonParseError::NoError) {
        setError(QString("transfer header: %1").arg(error.errorString()), true);
        return;
    }

    // If the device name was provided, use it
    mDeviceName = object.value("name").toString();
    if (!mDeviceName.isEmpty()) {
        emit q->deviceNameChanged(mDeviceName);
    }

    // Strings must be used for 64-bit numbers
    mItemCount = object.value("count").toString().toInt();
    mBytesTotal = object.value("size").toString().toLongLong();

    // Prepare to receive the first item
    mProtocolState = ItemHeader;
}

void TransferPrivate::processItemHeader(Packet *packet)
{
    QJsonParseError error;
    QJsonObject object = QJsonDocument::fromJson(packet->content(), &error).object();
    if (error.error != QJsonParseError::NoError) {
        setError(QString("item header: %1").arg(error.errorString()), true);
        return;
    }

    // In order to maintain compatibility with legacy versions (which is very
    // desirable), if "type" is not in the object, assume "file" unless
    // "directory" is present (in which case, use that)
    QString type;
    if (object.contains("type")) {
        type = object.value("type").toString();
    } else {
        if (object.contains("directory")) {
            type = "directory";
        } else {
            type = "file";
        }
    }

    // Attempt to locate a handler for the type
    Handler *handler = mApplication->handlerRegistry()->find(type);
    if (!handler) {
        setError(tr("unrecognized item type \"%1\"").arg(type), true);
        return;
    }

    // Use the handler to create an item and open it
    mCurrentItem = handler->createItem(type, object.toVariantMap());
    mCurrentItem->setParent(this);
    if (!mCurrentItem->open(Item::Write)) {
        setError(tr("unable to open \"%1\" for writing").arg(mCurrentItem->name()), true);
        return;
    }

    // Reset transfer stats
    mCurrentItemBytesTransferred = 0;
    mCurrentItemBytesTotal = mCurrentItem->size();

    // If the item has a size, switch states; otherwise receive the next item
    if (mCurrentItemBytesTotal) {
        mProtocolState = ItemContent;
    } else {
        processNext();
    }
}

void TransferPrivate::processItemContent(Packet *packet)
{
    mCurrentItem->write(packet->content());

    // Add the number of bytes to the global & current item totals
    mBytesTransferred += packet->content().size();
    mCurrentItemBytesTransferred += packet->content().size();
    mLastIntervalBytesTransferred += packet->content().size();

    updateProgress();

    // If the current item is complete, advance to the next item or finish
    if (mCurrentItemBytesTransferred >= mCurrentItemBytesTotal) {
        processNext();
    }
}

void TransferPrivate::processNext()
{
    // Close & free the current item and increment the index
    mCurrentItem->close();
    delete mCurrentItem;
    ++mItemIndex;

    // If there are no more items, send the success packet
    if (mItemIndex == mItemCount) {
        setSuccess(true);
    } else {
        mProtocolState = ItemHeader;
    }
}

void TransferPrivate::updateProgress()
{
    int newProgress = 0;
    if (mBytesTotal) {
        newProgress = static_cast<int>(100.0 *
            static_cast<double>(mBytesTransferred) /
            static_cast<double>(mBytesTotal));
    }

    // Only update progress if it has actually changed
    if (newProgress != mProgress) {
        emit q->progressChanged(mProgress = newProgress);
    }
}

void TransferPrivate::setSuccess(bool send)
{
    if (send) {
        Packet packet(Packet::Success);
        mTransport->sendPacket(&packet);
    }

    emit q->stateChanged(mState = Transfer::Succeeded);

    // Stop the speed timer
    mSpeedTimer.stop();

    // Both peers should be aware that the transfer succeeded at this point
    mTransport->close();
}

void TransferPrivate::setError(const QString &message, bool send)
{
    mApplication->logger()->log(new Message(
        Message::Error,
        MessageTag,
        message
    ));

    if (send) {
        Packet packet(Packet::Error, message.toUtf8());
        mTransport->sendPacket(&packet);
    }

    emit q->errorChanged(mError = message);
    emit q->stateChanged(mState = Transfer::Failed);

    // Stop the speed timer
    mSpeedTimer.stop();

    // An error on either end necessitates the transport be closed
    if (mTransport) {
        mTransport->close();
    }

    // The protocol dictates that the transfer is now "finished"
    mProtocolState = Finished;
}

void TransferPrivate::onConnected()
{
    emit q->stateChanged(mState = Transfer::InProgress);
    sendTransferHeader();

    // Start the speed timer
    mSpeedTimer.start(SpeedInterval);
}

void TransferPrivate::onPacketReceived(Packet *packet)
{
    // If an error packet is received, set the error and quit
    if (packet->type() == Packet::Error) {
        setError(packet->content());
        return;
    }

    if (mDirection == Transfer::Send) {

        // The only packet expected when sending items is the success packet
        // which indicates the receiver got all of the files
        if (mProtocolState == Finished && packet->type() == Packet::Success) {
            setSuccess();
            return;
        }

    } else {

        // Dispatch the packet to the appropriate method based on state
        switch (mProtocolState) {
        case TransferHeader:
            processTransferHeader(packet);
            return;
        case ItemHeader:
            processItemHeader(packet);
            return;
        case ItemContent:
            processItemContent(packet);
            return;
        case Finished:
            return;
        }
    }

    // Any other packet was unexpected - assume this is an error
    setError(tr("protocol error - unexpected packet"), true);
}

void TransferPrivate::onPacketSent()
{
    // We don't care about sent packets when receiving data
    if (mDirection == Transfer::Receive) {
        return;
    }

    switch (mProtocolState) {
    case TransferHeader:
        sendTransferHeader();
        break;
    case ItemHeader:
        sendItemHeader();
        break;
    case ItemContent:
        sendItemContent();
        break;
    case Finished:
        break;
    }
}

void TransferPrivate::onError(const QString &message)
{
    setError(message, true);
}

void TransferPrivate::onTimeout()
{
    auto curMs = QDateTime::currentMSecsSinceEpoch();

    // Use the time that has elapsed since the last interval to check the speed
    auto newSpeed = static_cast<qint64>(
        static_cast<double>(mLastIntervalBytesTransferred) /
        (static_cast<double>(curMs - mLastInterval) / 1000)
    );

    // If the speed differs from the previous value, emit a signal
    if (newSpeed != mSpeed) {
        emit q->speedChanged(mSpeed = newSpeed);
    }

    // Reset the calculation variables
    mLastInterval = curMs;
    mLastIntervalBytesTransferred = 0;
}

Transfer::Transfer(Application *application, Device *device, Bundle *bundle, QObject *parent)
    : QObject(parent),
      d(new TransferPrivate(this, application, device, nullptr, bundle))
{
}

Transfer::Transfer(Application *application, Transport *transport, QObject *parent)
    : QObject(parent),
      d(new TransferPrivate(this, application, nullptr, transport, nullptr))
{
}

Transfer::Direction Transfer::direction() const
{
    return d->mDirection;
}

Transfer::State Transfer::state() const
{
    return d->mState;
}

int Transfer::progress() const
{
    return d->mProgress;
}

qint64 Transfer::speed() const
{
    return d->mSpeed;
}

qint64 Transfer::bytesRemaining() const
{
    return d->mBytesTotal - d->mBytesTransferred;
}

QString Transfer::deviceName() const
{
    return d->mDeviceName;
}

QString Transfer::error() const
{
    return d->mError;
}

bool Transfer::isFinished() const
{
    return d->mState == Failed || d->mState == Succeeded;
}

void Transfer::cancel()
{
    d->setError(tr("transfer cancelled"), true);
}
