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

#include <QApplication>
#include <QFileDialog>

#include "../device/devicedialog.h"
#include "../icon/trayicon.h"
#include "application.h"

#ifdef BUILD_APPINDICATOR
#include "../icon/indicatoricon.h"
#include "../util/platform.h"
#endif

// Enable QHostAddress to be used in a QVariant
Q_DECLARE_METATYPE(QHostAddress)

Application::Application()
    : mTransferWindow(&mTransferModel),
#ifdef BUILD_APPINDICATOR
      mIcon(Platform::isUnity() ? static_cast<Icon *>(new IndicatorIcon) :
                                  static_cast<Icon *>(new TrayIcon))
#else
      mIcon(new TrayIcon)
#endif
{
    connect(&mDeviceModel, &DeviceModel::rowsInserted, this, &Application::notifyDevicesAdded);
    connect(&mDeviceModel, &DeviceModel::rowsRemoved, this, &Application::notifyDevicesRemoved);
    connect(&mTransferServer, &TransferServer::newTransfer, &mTransferModel, &TransferModel::add);

    mIcon->addAction(tr("Send Files..."), this, SLOT(sendFiles()));
    mIcon->addAction(tr("Send Directory..."), this, SLOT(sendDirectory()));
    mIcon->addSeparator();
    mIcon->addAction(tr("View Transfers..."), &mTransferWindow, SLOT(show()));
    mIcon->addSeparator();
    mIcon->addAction(tr("Exit"), QApplication::instance(), SLOT(quit()));
}

void Application::notifyDevicesAdded(const QModelIndex &, int first, int last)
{
    for(int row = first; row <= last; ++row) {
        mIcon->showMessage(tr("%1 has joined.").arg(
            mDeviceModel.data(mDeviceModel.index(row, 0), DeviceModel::NameRole).toString()
        ));
    }
}

void Application::notifyDevicesRemoved(const QModelIndex &, int first, int last)
{
    for(int row = first; row <= last; ++row) {
        mIcon->showMessage(tr("%1 has left.").arg(
            mDeviceModel.data(mDeviceModel.index(row, 0), DeviceModel::NameRole).toString()
        ));
    }
}

void Application::sendFiles()
{
    QStringList filenames(QFileDialog::getOpenFileNames(nullptr, tr("Select Files")));

    if(filenames.length()) {
        BundlePointer bundle(new Bundle);

        foreach(QString filename, filenames) {
            bundle->addFile(filename);
        }

        sendBundle(bundle);
    }
}

void Application::sendDirectory()
{
    QString path(QFileDialog::getExistingDirectory(nullptr, tr("Select Directory")));

    if(!path.isNull()) {
        BundlePointer bundle(new Bundle);

        bundle->addDirectory(path);
        sendBundle(bundle);
    }
}

void Application::sendBundle(BundlePointer bundle)
{
    QModelIndex index = DeviceDialog::getDevice(&mDeviceModel);
    if(index.isValid()) {
        QString deviceName = index.data(DeviceModel::NameRole).value<QString>();
        QHostAddress address = index.data(DeviceModel::AddressRole).value<QHostAddress>();
        quint16 port = index.data(DeviceModel::PortRole).value<quint16>();

        // The constructor used below creates an outgoing transfer
        Transfer *transfer = new Transfer(deviceName, address, port, bundle);
        mTransferModel.add(transfer);
    }
}
