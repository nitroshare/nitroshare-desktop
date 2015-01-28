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
#include <QIcon>
#include <QMenu>

#include "device/device.h"
#include "device/devicedialog.h"
#include "nitroshare.h"

NitroShare::NitroShare()
    : mTransferWindow(mTransferModel)
{
    connect(&mDeviceModel, &DeviceModel::deviceAdded, this, &NitroShare::notifyDeviceAdded);
    connect(&mDeviceModel, &DeviceModel::deviceRemoved, this, &NitroShare::notifyDeviceRemoved);
    connect(&mTransferServer, &TransferServer::newTransfer, &mTransferModel, &TransferModel::add);

    initializeMenu();

    setIcon(QIcon(":/img/icon.png"));
    setContextMenu(&mMenu);

    show();

    mDeviceModel.start();
}

void NitroShare::notifyDeviceAdded(DevicePointer device)
{
    showMessage(tr("Device Added"), device->name());
}

void NitroShare::notifyDeviceRemoved(DevicePointer device)
{
    showMessage(tr("Device Removed"), device->name());
}

void NitroShare::sendFiles()
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

void NitroShare::sendDirectory()
{
    QString path(QFileDialog::getExistingDirectory(nullptr, tr("Select Directory")));

    if(!path.isNull()) {
        BundlePointer bundle(new Bundle);

        bundle->addDirectory(path);
        sendBundle(bundle);
    }
}

void NitroShare::initializeMenu()
{
    mMenu.addAction(tr("Send &Files..."), this, SLOT(sendFiles()));
    mMenu.addAction(tr("Send &Directory..."), this, SLOT(sendDirectory()));
    mMenu.addSeparator();
    mMenu.addAction(tr("View &Transfers..."), &mTransferWindow, SLOT(show()));
    mMenu.addSeparator();
    mMenu.addAction(tr("E&xit"), QApplication::instance(), SLOT(quit()));
}

void NitroShare::sendBundle(BundlePointer bundle)
{
    DevicePointer device(DeviceDialog::getDevice(mDeviceModel));
    if(device) {
        TransferPointer transfer(new Transfer(device, bundle));
        mTransferModel.add(transfer);
    }
}
