/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Nathan Osman
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

class Application;
class Bundle;
class Device;
class Item;
class Packet;
class Transport;

class TransferPrivate : public QObject
{
    Q_OBJECT

public:

    TransferPrivate(Transfer *transfer,
                    Application *mApplication,
                    Device *device,
                    Transport *mTransport,
                    Bundle *mBundle);

    void sendTransferHeader();
    void sendItemHeader();
    void sendItemContent();
    void sendNext();

    void processTransferHeader(Packet *packet);
    void processItemHeader(Packet *packet);
    void processItemContent(Packet *packet);
    void processNext();

    void updateProgress();

    void setSuccess(bool send = false);
    void setError(const QString &message, bool send = false);

    Transfer *const q;

    Application *mApplication;
    Transport *mTransport;
    Bundle *mBundle;

    enum {
        TransferHeader,
        ItemHeader,
        ItemContent,
        Finished
    } mProtocolState;

    Transfer::Direction mDirection;
    Transfer::State mState;
    int mProgress;
    QString mDeviceName;
    QString mError;

    qint32 mItemIndex;
    qint32 mItemCount;
    qint64 mBytesTransferred;
    qint64 mBytesTotal;

    Item *mCurrentItem;
    qint64 mCurrentItemBytesTransferred;
    qint64 mCurrentItemBytesTotal;

public Q_SLOTS:

    void onConnected();
    void onPacketReceived(Packet *packet);
    void onPacketSent();
    void onError(const QString &message);
};

#endif // LIBNITROSHARE_TRANSFER_P_H
