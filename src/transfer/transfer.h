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

#include <QMutex>
#include <QMutexLocker>
#include <QSharedPointer>
#include <QTcpSocket>

class Transfer : public QObject
{
    Q_OBJECT

public:

    Transfer();

    QString deviceName() const {
        QMutexLocker locker(&mMutex);
        return mDeviceName;
    }
    int progress() const {
        QMutexLocker locker(&mMutex);
        return mProgress;
    }

signals:

    void deviceNameChanged(const QString &deviceName);
    void progressChanged(int progress);

    void error(const QString &message);
    void complete();

    void finished();

public slots:

    void start();
    void cancel();

protected slots:

    virtual void performTransfer() = 0;

protected:

    QTcpSocket mSocket;
    mutable QMutex mMutex;

    QString mDeviceName;
    int mProgress;
    bool mCancelled;
};

typedef QSharedPointer<Transfer> TransferPointer;
Q_DECLARE_METATYPE(TransferPointer)

#endif // NS_TRANSFER_H
