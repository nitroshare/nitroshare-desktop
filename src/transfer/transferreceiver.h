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
#include <QTemporaryDir>

#include "transfer.h"

class TransferReceiver : public Transfer
{
    Q_OBJECT

public:

    TransferReceiver(QSslConfiguration *configuration, qintptr socketDescriptor);
    virtual ~TransferReceiver();

    virtual void startConnect();

    /**
     * @brief Accept quarantined files
     *
     * If quarantining files is enabled, they are stored in a temporary
     * directory until this method is invoked. The files are then copied into
     * the storage directory.
     */
    void accept();

private:

    virtual void startTransfer();

    virtual void processJsonPacket(const QJsonObject &object);
    virtual void processBinaryPacket(const QByteArray &data);

    virtual void writeNextPacket();

    void processTransferHeader(const QJsonObject &object);
    void processItemHeader(const QJsonObject &object);

    void nextItem();
    bool openFile();

    // Information for writing files that are received
    QDir mRoot;
    const QDir mTransferDirectory;
    QTemporaryDir *mTempDir;
    const bool mShouldQuarantine;
    const bool mOverwrite;

    // Number of items remaining to be transferred
    qint32 mTransferItemsRemaining;

    // Data needed for the file currently being read
    QFile mFile;
    qint64 mFileBytesRemaining;

    // Error message pending
    QString mError;
};

#endif // NS_TRANSFERRECEIVER_H
