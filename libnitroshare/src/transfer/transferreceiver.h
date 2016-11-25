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

#ifndef LIBNITROSHARE_TRANSFERRECEIVER_H
#define LIBNITROSHARE_TRANSFERRECEIVER_H

#include "transfer.h"

class QJsonObject;

class Transport;

/**
 * @brief Transfer initiated by a remote peer to receive items
 *
 * Raw data from the transfer is consumed by onDataReceived() as it is
 * received from the transport. From there, it is dispatched to the
 * appropriate method based on the current protocol state.
 */
class TransferReceiver : public Transfer
{
    Q_OBJECT

public:

    /**
     * @brief Create a transfer to receive items
     * @param transport pointer to Transport
     */
    TransferReceiver(Transport *transport);

private Q_SLOTS:

    void onDataReceived(const QByteArray &data);

private:

    enum { InvalidSize = -1 };

    void processPacket(const QByteArray &packet);

    qint32 mNextPacketSize;
    QByteArray mReadBuffer;
};

#endif // LIBNITROSHARE_TRANSFERRECEIVER_H
