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

#ifndef NS_TRANSFER_H
#define NS_TRANSFER_H

#include <QHostAddress>
#include <QTcpSocket>
#include <QVariantMap>

#include "../bundle/bundle.h"
#include "transfermodel.h"

class Transfer : public QObject
{
    Q_OBJECT

public:

    // "explicit" is not needed here since this class is abstract
    Transfer(TransferModel::Direction direction);

    QString deviceName() const {
        return mDeviceName;
    }

    int progress() const {
        return mProgress;
    }

    TransferModel::Direction direction() const {
        return mDirection;
    }

    TransferModel::State state() const {
        return mState;
    }

    QString error() const {
        return mError;
    }

    virtual void start() = 0;

    void cancel();
    void restart();

Q_SIGNALS:

    void dataChanged(const QVector<int> &roles = QVector<int>());

private Q_SLOTS:

    void onConnected();
    void onReadyRead();
    void onBytesWritten();
    void onError(QAbstractSocket::SocketError error);

protected:

    virtual void processPacket(const QByteArray &data) = 0;
    virtual void writeNextPacket() = 0;

    void writePacket(const QByteArray &data);
    void writePacket(const QVariantMap &map);

    void calculateProgress();
    void abortWithError(const QString &message);
    void finish();

    QTcpSocket mSocket;

    enum {
        TransferHeader,
        FileHeader,
        FileData,
        Finished
    } mProtocolState;

    QString mDeviceName;

    qint64 mTransferBytes;
    qint64 mTransferBytesTotal;

private:

    void reset();

    const TransferModel::Direction mDirection;
    TransferModel::State mState;
    QString mError;

    int mProgress;

    QByteArray mBuffer;
    qint32 mBufferSize;
};

#endif // NS_TRANSFER_H
