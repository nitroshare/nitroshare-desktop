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

#ifndef NS_TRANSFERRECEIVER_H
#define NS_TRANSFERRECEIVER_H

#include <QDir>
#include <QFile>

#include "transfer.h"

class TransferReceiver : public Transfer
{
    Q_OBJECT

public:

    explicit TransferReceiver(qintptr socketDescriptor);

    virtual void start();

private:

    virtual bool processPacket(const QByteArray &data);
    virtual void writeNextPacket();

    bool processTransferHeader(const QByteArray &data);
    bool processItemHeader(const QByteArray &data);
    bool processItemData(const QByteArray &data);

    void nextItem();

    // Directory to write files to
    QDir mRoot;

    // Number of items remaining to be transferred
    qint32 mTransferItemsRemaining;

    // Data needed for the file currently being read
    QFile mFile;
    qint64 mFileBytesRemaining;
};

#endif // NS_TRANSFERRECEIVER_H
