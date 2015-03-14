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

#ifndef NS_TRANSFERSENDER_H
#define NS_TRANSFERSENDER_H

#include <QHostAddress>
#include <QList>

#include "../bundle/bundle.h"
#include "../bundle/bundleitem.h"
#include "transfer.h"

class TransferSender : public Transfer
{
    Q_OBJECT

public:

    TransferSender(const QString &deviceName, const QHostAddress &address, quint16 port, const Bundle *bundle);
    virtual ~TransferSender();

    virtual void start();

private:

    virtual void processJsonPacket(const QJsonObject &object);
    virtual void processBinaryPacket(const QByteArray &data);

    virtual void writeNextPacket();

    void writeTransferHeader();
    void writeItemHeader();
    void writeItemData();

    void nextItem();

    const QHostAddress mAddress;
    const quint16 mPort;
    const Bundle *mBundle;

    const int mBufferSize;

    QList<BundleItem>::const_iterator mCurrentItem;
};

#endif // NS_TRANSFERSENDER_H
