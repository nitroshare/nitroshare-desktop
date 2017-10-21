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
#include <QFileInfo>

#include <nitroshare/action.h>
#include <nitroshare/actionregistry.h>
#include <nitroshare/application.h>

#include "sendfilesaction.h"

SendFilesAction::SendFilesAction(Application *application)
    : mApplication(application)
{
}

QString SendFilesAction::name() const
{
    return "sendfiles";
}

bool SendFilesAction::ui() const
{
    return true;
}

QString SendFilesAction::title() const
{
    return tr("Send Files...");
}

QVariant SendFilesAction::invoke(const QVariantMap &)
{
    // Obtain the list of files from the user
    QStringList filenames(QFileDialog::getOpenFileName(nullptr, tr("Select Files")));
    if (!filenames.count()) {
        return false;
    }

    // Find and invoke the browse action to obtain a device name
    Action *browseAction = mApplication->actionRegistry()->find("browse");
    QVariant deviceName = browseAction->invoke();
    if (deviceName.type() != QVariant::String) {
        return false;
    }

    // Find & invoke the send action - it's a dependency, so must exist
    Action *sendAction = mApplication->actionRegistry()->find("send");
    return sendAction->invoke({
        { "device", deviceName },
        { "root", QFileInfo(filenames.at(0)).path() },
        { "items", filenames }
    });
}
