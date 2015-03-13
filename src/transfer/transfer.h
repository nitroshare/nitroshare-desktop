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

#include <QFile>
#include <QJsonObject>
#include <QTcpSocket>

#include "transfermodel.h"

/**
 * @brief A transfer between two devices
 *
 * This class facilitates the actual transfer of files and directories between
 * devices. Data is exchanged using "packets", which are distinct pieces of
 * information. Packets can contain metadata, file contents, or error message,
 * for example.
 *
 * At the beginning of the transfer, the device name, number of items, and
 * total size is sent. Then, for each item, its metadata and contents are
 * transferred. Once this is complete, an acknowledgement packet is sent.
 *
 * At a lower level, a packet consists of a 32-bit signed integer indicating
 * its size. The first byte of the packet indicates its type. Four types are
 * currently used - success, error, JSON, and binary packets.
 */
class Transfer : public QObject
{
    Q_OBJECT

public:

    // "explicit" is not needed here since this class is abstract
    Transfer(TransferModel::Direction direction);

    QString deviceName() const { return mDeviceName; }
    int progress() const { return mProgress; }
    TransferModel::Direction direction() const { return mDirection; }
    TransferModel::State state() const { return mState; }
    QString error() const { return mError; }

    virtual void start() = 0;

    void cancel();
    void restart();

Q_SIGNALS:

    void dataChanged(const QVector<int> &roles = {});

private Q_SLOTS:

    void onConnected();
    void onReadyRead();
    void onBytesWritten();
    void onError(QAbstractSocket::SocketError error);

protected:

    virtual void processJsonPacket(const QJsonObject &object) = 0;
    virtual void processBinaryPacket(const QByteArray &data) = 0;

    virtual void writeNextPacket() = 0;

    void writeSuccessPacket();
    void writeErrorPacket(const QString &message);
    void writeJsonPacket(const QJsonObject &object);
    void writeBinaryPacket(const QByteArray &data);

    void updateProgress();

    QTcpSocket mSocket;
    QString mDeviceName;

    enum class ProtocolState {
        TransferHeader,
        TransferItems,
        Acknowledgement,
        Finished
    } mProtocolState;

    qint64 mTransferBytes;
    qint64 mTransferBytesTotal;

    QFile mFile;
    qint64 mFileBytesRemaining;

private:

    enum class PacketType : char {
        Success = 0,
        Error,
        Json,
        Binary
    };

    void processPacket();
    void writePacket(PacketType type, const QByteArray &data = QByteArray());

    void reset();
    void finish(TransferModel::State state);

    const TransferModel::Direction mDirection;
    TransferModel::State mState;
    QString mError;

    int mProgress;

    QByteArray mBuffer;
    qint32 mBufferSize;
};

#endif // NS_TRANSFER_H
