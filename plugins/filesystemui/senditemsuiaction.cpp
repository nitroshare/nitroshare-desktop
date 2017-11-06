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

#include <QFileDialog>

#include <nitroshare/action.h>
#include <nitroshare/actionregistry.h>
#include <nitroshare/application.h>

#include "senditemsuiaction.h"

SendItemsUiAction::SendItemsUiAction(Application *application, Type type)
    : mApplication(application),
      mType(type)
{
}

QString SendItemsUiAction::name() const
{
    return mType == SendFiles ? "sendfilesui" : "senddirectoryui";
}

bool SendItemsUiAction::api() const
{
    return true;
}

QString SendItemsUiAction::title() const
{
    return mType == SendFiles ?
        tr("select files and send them to a device") :
        tr("select directory and send it to a device");
}

QString SendItemsUiAction::description() const
{
    return tr(
        "Show a dialog for selecting items and send them to a device. "
        "A device selection dialog is shown after items are selected. "
        "This action takes no parameters and returns a boolean set to true if "
        "a transfer was created."
    );
}

bool SendItemsUiAction::menu() const
{
    return true;
}

QString SendItemsUiAction::label() const
{
    return mType == SendFiles ? tr("Send files...") : tr("Send directory...");
}

QVariant SendItemsUiAction::invoke(const QVariantMap &)
{
    QStringList items;

    // Show the appropriate dialog
    if (mType == SendFiles) {
        items = QFileDialog::getOpenFileNames(nullptr, tr("Select Files"));
        if (!items.count()) {
            return false;
        }
    } else {
        QString item = QFileDialog::getExistingDirectory(nullptr, tr("Select Directory"));
        if (item.isNull()) {
            return false;
        }
        items = QStringList{ item };
    }

    // Invoke the device selection dialog to select a device
    QVariant deviceRet = mApplication->actionRegistry()->find("selectdeviceui")->invoke();
    if (deviceRet.type() != QVariant::Map) {
        return false;
    }

    // Invoke the send files action to initiate the transfer
    return mApplication->actionRegistry()->find("sendfiles")->invoke({
        { "device", deviceRet.toMap().value("device").toString() },
        { "enumerator", deviceRet.toMap().value("enumerator").toString() },
        { "items", items },
    });
}
