/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Nathan Osman
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

#include "senddirectoryuiaction.h"

SendDirectoryUiAction::SendDirectoryUiAction(Application *application)
    : mApplication(application)
{
}

QString SendDirectoryUiAction::name() const
{
    return "senddirectoryui";
}

bool SendDirectoryUiAction::api() const
{
    return true;
}

bool SendDirectoryUiAction::menu() const
{
    return true;
}

QString SendDirectoryUiAction::label() const
{
    return tr("Send directory...");
}

QString SendDirectoryUiAction::description() const
{
    return tr(
        "Select a directory and a device to send it to and initiate a transfer. "
        "This action takes no parameters and returns a boolean set to true if "
        "a transfer was created."
    );
}

QVariant SendDirectoryUiAction::invoke(const QVariantMap &)
{
    // Show the directory selection dialog
    QString item = QFileDialog::getExistingDirectory(nullptr, tr("Select Directory"));
    if (item.isNull()) {
        return false;
    }

    // Use the senditemsui action to prompt for a device and send the directory
    // It is safe to assume the action exists since this plugin provides it
    return mApplication->actionRegistry()->find("senditemsui")->invoke({
        { "items", QStringList{ item } }
    });
}
