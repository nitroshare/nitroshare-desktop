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

#ifndef LIBNITROSHARE_TRANSFER_P_H
#define LIBNITROSHARE_TRANSFER_P_H

#include <QObject>

#include <nitroshare/transfer.h>

class QIODevice;
class QJsonObject;

class Bundle;
class HandlerRegistry;
class Item;
class Settings;
class Transport;

class TransferPrivate : public QObject
{
    Q_OBJECT

public:

    enum class PacketType : char {
        Success = 0,
        Error,
        Json,
        Binary
    };

    TransferPrivate(Transfer *parent, Settings *settings, HandlerRegistry *handlerRegistry,
                    Transport *transport, Bundle *bundle, Transfer::Direction direction);

    void sendPacket(PacketType packetType, const QByteArray &packet = QByteArray());

    void processPacket(PacketType packetType, const QByteArray &packet);
    void processTransferHeader(const QJsonObject &object);
    void processItemHeader(const QJsonObject &object);
    void processItemContent(const QByteArray &packet);

    void updateProgress();

    void setSuccess(bool send = false);
    void setError(const QString &message, bool send = false);

    Transfer *const q;

    Settings *settings;
    HandlerRegistry *handlerRegistry;
    Transport *transport;
    Bundle *bundle;

    enum {
        TransferHeader,
        ItemHeader,
        ItemContent,
        Finished
    } protocolState;

    Transfer::Direction direction;
    Transfer::State state;
    int progress;
    QString deviceName;
    QString error;

    qint32 itemIndex;
    qint32 itemCount;
    qint64 bytesTransferred;
    qint64 bytesTotal;

    qint32 nextPacketSize;
    QByteArray readBuffer;

    Item *currentItem;
    QIODevice *currentDevice;

    qint64 currentItemBytesTransferred;
    qint64 currentItemBytesTotal;

public Q_SLOTS:

    void onDataReceived(const QByteArray &data);
    void onDataWritten(qint64 bytes);
    void onError(const QString &message);
};

#endif // LIBNITROSHARE_TRANSFER_P_H
