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

#include "discovery/device.h"
#include "filesystem/bundle.h"
#include "nitroshare.h"

NitroShare::NitroShare()
{
    connect(&mDeviceModel, &DeviceModel::rowsInserted, this, &NitroShare::notifyDevicesAdded);
    connect(&mDeviceModel, &DeviceModel::rowsAboutToBeRemoved, this, &NitroShare::notifyDevicesRemoved);

    initMenu();

    setIcon(QIcon(":/img/icon.png"));
    setContextMenu(&mMenu);

    show();

    mDeviceModel.start();
}

void NitroShare::notifyDevicesAdded(const QModelIndex &parent, int first, int last)
{
    for(int i = first; i < last; ++i) {
        DevicePointer device = parent.data(Qt::UserRole).value<DevicePointer>();
        showMessage(tr("Device Added"), device->name());
    }
}

void NitroShare::notifyDevicesRemoved(const QModelIndex &parent, int first, int last)
{
    for(int i = first; i < last; ++i) {
        DevicePointer device = parent.data(Qt::UserRole).value<DevicePointer>();
        showMessage(tr("Device Removed"), device->name());
    }
}

void NitroShare::sendFiles()
{
    QStringList filenames = QFileDialog::getOpenFileNames(nullptr, tr("Select Files"));

    if(filenames.length()) {
        Bundle bundle;

        foreach(QString filename, filenames) {
            bundle.addFile(filename);
        }

        //...
    }
}

void NitroShare::sendDirectory()
{
    QString path = QFileDialog::getExistingDirectory(nullptr, tr("Select Directory"));

    if(!path.isNull()) {
        Bundle bundle;
        bundle.addDirectory(path);

        //...
    }
}

void NitroShare::initMenu()
{
    mMenu.addAction(tr("Send &Files..."), this, SLOT(sendFiles()));
    mMenu.addAction(tr("Send &Directory..."), this, SLOT(sendDirectory()));
    mMenu.addSeparator();
    mMenu.addAction(tr("E&xit"), QApplication::instance(), SLOT(quit()));
}
