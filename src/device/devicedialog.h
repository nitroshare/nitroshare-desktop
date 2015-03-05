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

#ifndef NS_DEVICEDIALOG_H
#define NS_DEVICEDIALOG_H

#include <QDialog>
#include <QItemSelection>

#include "devicemodel.h"
#include "ui_devicedialog.h"

/**
 * @brief Dialog for selecting a device
 *
 * Given a DeviceModel instance, this dialog will display the devices
 * currently in the model and allow the user to select one. It is not
 * necessary to create an instance of the dialog in order to obtain a
 * selection from the user - simply use the getDevice() static method.
 */
class DeviceDialog : public QDialog, public Ui::DeviceDialog
{
    Q_OBJECT

public:

    explicit DeviceDialog(DeviceModel *model);

    QModelIndex selectedDeviceIndex() const;

    static QModelIndex getDevice(DeviceModel *model);

private Q_SLOTS:

    void toggleOkButton(const QItemSelection &selected, const QItemSelection &deselected);
};

#endif // NS_DEVICEDIALOG_H
