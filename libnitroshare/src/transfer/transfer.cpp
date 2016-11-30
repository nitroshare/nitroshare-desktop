/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Nathan Osman
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

#include <QIODevice>
#include <QJsonDocument>
#include <QJsonObject>
#include <QtEndian>

#include <nitroshare/bundle.h>
#include <nitroshare/handler.h>
#include <nitroshare/handlerregistry.h>
#include <nitroshare/item.h>
#include <nitroshare/packet.h>
#include <nitroshare/transfer.h>
#include <nitroshare/transport.h>

#include "transfer_p.h"

TransferPrivate::TransferPrivate(Transfer *parent, HandlerRegistry *handlerRegistry, Transport *transport,
                                 Bundle *bundle, Transfer::Direction direction)
    : QObject(parent),
      q(parent),
      handlerRegistry(handlerRegistry),
      transport(transport),
      bundle(bundle),
      protocolState(TransferHeader),
      direction(direction),
      state(Transfer::Connecting),
      progress(0),
      itemIndex(0),
      itemCount(bundle ? bundle->rowCount(QModelIndex()) : 0),
      bytesTransferred(0),
      bytesTotal(bundle ? bundle->totalSize() : 0),
      nextPacketSize(0),
      currentItem(nullptr),
      currentDevice(nullptr),
      currentItemBytesTransferred(0),
      currentItemBytesTotal(0)
{
    // If sending data, trigger the first packet after connection
    if (direction == Transfer::Send) {
        connect(transport, &Transport::connected, [this]() {
            onPacketSent();
        });
    }

    connect(transport, &Transport::packetReceived, this, &TransferPrivate::onPacketReceived);
    connect(transport, &Transport::packetSent, this, &TransferPrivate::onPacketSent);
    connect(transport, &Transport::error, this, &TransferPrivate::onError);
}

void TransferPrivate::processTransferHeader(const QJsonObject &object)
{
    deviceName = object.value("name").toString();
    if (!deviceName.isEmpty()) {
        emit q->deviceNameChanged(deviceName);
    }

    itemCount = object.value("count").toString().toInt();
    bytesTotal = object.value("size").toString().toLongLong();

    // Prepare to receive the first item
    protocolState = ItemHeader;
}

void TransferPrivate::processItemHeader(const QJsonObject &object)
{
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
    Handler *handler = handlerRegistry->handlerForType(type);
    if (!handler) {
        setError(tr("unrecognized item type \"%1\"").arg(type));
        return;
    }

    // Use the handler to create an item and open it
    currentItem = handler->createItem(type, object.toVariantMap());
    currentItem->setParent(this);
    currentDevice = currentItem->createWriter();
    if (!currentDevice) {
        setError(tr("unable to write \"%1\"").arg(object.value("name").toString()));
        return;
    }

    // Record the size of the item
    currentItemBytesTransferred = 0;
    currentItemBytesTotal = object.value("size").toString().toLongLong();

    // Prepare to receive item contents (if size > 0)
    if (currentItemBytesTotal) {
        protocolState = ItemContent;
    }
}

void TransferPrivate::processItemContent(const QByteArray &content)
{
    currentDevice->write(content);

    // Add the number of bytes to the global & current item totals
    bytesTransferred += content.size();
    currentItemBytesTransferred -= content.size();

    updateProgress();

    // If the current item is complete, advance to the next item or finish
    if (currentItemBytesTransferred >= currentItemBytesTotal) {

        // Free the current item (device is a child and also deleted)
        delete currentItem;

        // Increment the index of the next item to transfer
        ++itemIndex;

        // If there are no more items, send the success packet
        if (itemIndex == itemCount) {
            setSuccess(true);
        } else {
            protocolState = ItemHeader;
        }
    }
}

void TransferPrivate::updateProgress()
{
    int oldProgress = progress;
    int newProgress = static_cast<int>(100.0 * bytesTotal ?
        static_cast<double>(bytesTransferred) / static_cast<double>(bytesTotal) : 0.0);

    // Only update progress if it has actually changed
    if (newProgress != oldProgress) {
        emit q->progressChanged(progress = newProgress);
    }
}

void TransferPrivate::setSuccess(bool send)
{
    if (send) {
        Packet packet(Packet::Success);
        transport->sendPacket(&packet);
    }

    emit q->stateChanged(state = Transfer::Succeeded);

    // Both peers should be aware that the transfer succeeded at this point
    transport->close();
}

void TransferPrivate::setError(const QString &message, bool send)
{
    if (send) {
        Packet packet(Packet::Error, message.toUtf8());
        transport->sendPacket(&packet);
    }

    emit q->errorChanged(error = message);
    emit q->stateChanged(state = Transfer::Failed);

    transport->close();
}

void TransferPrivate::onPacketReceived(Packet *packet)
{
    // If an error packet is received, set the error and quit
    if (packet->type() == Packet::Error) {
        setError(packet->content());
        return;
    }

    if (direction == Transfer::Send) {

        // The only packet expected when sending items is the success packet
        // which indicates the receiver got all of the files
        if (protocolState == Finished && packet->type() == Packet::Success) {
            setSuccess();
            return;
        }

    } else {

        // Dispatch the packet to the appropriate method based on state
        switch (protocolState) {
        case TransferHeader:
            processTransferHeader(QJsonDocument::fromJson(packet->content()).object());
            return;
        case ItemHeader:
            processItemHeader(QJsonDocument::fromJson(packet->content()).object());
            return;
        case ItemContent:
            processItemContent(packet->content());
            return;
        }
    }

    // Any other packet was unexpected - assume this is an error
    setError(tr("protocol error - unexpected packet"), true);
}

void TransferPrivate::onPacketSent()
{
    //...
}

void TransferPrivate::onError(const QString &message)
{
    setError(message);
}

Transfer::Transfer(HandlerRegistry *handlerRegistry, Transport *transport, QObject *parent)
    : QObject(parent),
      d(new TransferPrivate(this, handlerRegistry, transport, nullptr, Transfer::Receive))
{
}

Transfer::Transfer(HandlerRegistry *handlerRegistry, Transport *transport, Bundle *bundle, QObject *parent)
    : QObject(parent),
      d(new TransferPrivate(this, handlerRegistry, transport, bundle, Transfer::Send))
{
}

Transfer::Direction Transfer::direction() const
{
    return d->direction;
}

Transfer::State Transfer::state() const
{
    return d->state;
}

int Transfer::progress() const
{
    return d->progress;
}

QString Transfer::deviceName() const
{
    return d->deviceName;
}

QString Transfer::error() const
{
    return d->error;
}

void Transfer::cancel()
{
    d->setError(tr("transfer cancelled"));
}
