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

#ifndef NS_APPLICATION_H
#define NS_APPLICATION_H

#include <QModelIndex>

#include "../device/device.h"
#include "../device/devicemodel.h"
#include "../filesystem/bundle.h"
#include "../icon/icon.h"
#include "../transfer/transfermodel.h"
#include "../transfer/transferserver.h"
#include "../transfer/transferwindow.h"

class Application : public QObject
{
    Q_OBJECT

public:

    explicit Application(QObject *parent = nullptr);
    virtual ~Application();

private Q_SLOTS:

    void notifyDevicesAdded(const QModelIndex &parent, int first, int last);
    void notifyDevicesRemoved(const QModelIndex &parent, int first, int last);

    void sendFiles();
    void sendDirectory();

    void about();
    void aboutQt();

private:

    void sendBundle(BundlePointer bundle);

    DeviceModel mDeviceModel;
    TransferModel mTransferModel;
    TransferServer mTransferServer;
    TransferWindow mTransferWindow;

    Icon *mIcon;

    qint64 mStartTime;
};

#endif // NS_APPLICATION_H
