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

#include <QPointer>

#include "../bundle/bundle.h"
#include "../device/devicemodel.h"
#include "../icon/icon.h"
#include "../transfer/transfermodel.h"
#include "../transfer/transferserver.h"
#include "../transfer/transferwindow.h"

#ifdef BUILD_UPDATECHECKER
#include "updatechecker.h"
#endif

class Application : public QObject
{
    Q_OBJECT

public:

    Application();
    virtual ~Application();

private Q_SLOTS:

    void notifyDevicesAdded(const QModelIndex &parent, int first, int last);
    void notifyDevicesRemoved(const QModelIndex &parent, int first, int last);
    void notifyTransfersChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles);
    void notifyError(const QString &message);

#ifdef BUILD_UPDATECHECKER
    void onConfigureUpdateChecker();
    void notifyNewVersion(const QString &version, const QUrl &url);
#endif

    void sendFiles();
    void sendDirectory();

    void onOpenSettings();
    void onOpenAbout();
    void onOpenAboutQt();

private:

    void sendBundle(const Bundle *bundle);

    DeviceModel mDeviceModel;
    TransferModel mTransferModel;
    TransferServer mTransferServer;
    TransferWindow mTransferWindow;

#ifdef BUILD_UPDATECHECKER
    QPointer<UpdateChecker> mUpdateChecker;
#endif

    Icon *mIcon;

    qint64 mStartTime;
};

#endif // NS_APPLICATION_H
