/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Nathan Osman
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

#include <QDialogButtonBox>
#include <QLabel>
#include <QListView>
#include <QVBoxLayout>

#include <nitroshare/application.h>
#include <nitroshare/device.h>
#include <nitroshare/deviceenumerator.h>
#include <nitroshare/devicemodel.h>

#include "devicedialog.h"
#include "deviceproxymodel.h"

DeviceDialog::DeviceDialog(Application *application)
{
    setWindowTitle(tr("Select Device"));

    QLabel *label = new QLabel(tr("Select a device from the list below:"));
    QListView *listView = new QListView;
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    // Respond to the dialog buttons correctly
    connect(buttonBox, &QDialogButtonBox::rejected, this, &DeviceDialog::reject);
    connect(buttonBox, &QDialogButtonBox::accepted, [this, application, listView]() {
        Device *device = listView->currentIndex().data(Qt::UserRole).value<Device*>();
        mDeviceUuid = device->uuid();
        //mEnumeratorName = application->deviceModel()->enumeratorForDevice(device)->name();
        accept();
    });

    mModel.setSourceModel(application->deviceModel());
    listView->setModel(&mModel);

    // Add the widgets to the dialog
    QVBoxLayout *vboxLayout = new QVBoxLayout;
    vboxLayout->addWidget(label);
    vboxLayout->addWidget(listView);
    vboxLayout->addWidget(buttonBox);
    setLayout(vboxLayout);
}

QString DeviceDialog::deviceUuid() const
{
    return mDeviceUuid;
}

QString DeviceDialog::enumeratorName() const
{
    return mEnumeratorName;
}
