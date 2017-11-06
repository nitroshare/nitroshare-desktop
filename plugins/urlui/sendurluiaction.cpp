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

#include <QInputDialog>

#include <nitroshare/action.h>
#include <nitroshare/actionregistry.h>
#include <nitroshare/application.h>

#include "sendurluiaction.h"

SendUrlUiAction::SendUrlUiAction(Application *application)
    : mApplication(application)
{
}

QString SendUrlUiAction::name() const
{
    return "sendurlui";
}

bool SendUrlUiAction::menu() const
{
    return true;
}

QString SendUrlUiAction::label() const
{
    return tr("Send URL...");
}

QVariant SendUrlUiAction::invoke(const QVariantMap &)
{
    // First obtain the URL to send
    QString url = QInputDialog::getText(
        nullptr,
        tr("Send URL"),
        tr("Enter the URL to send")
    );
    if (url.isNull()) {
        return false;
    }

    // Browse for a device
    Action *browseAction = mApplication->actionRegistry()->find("browse");
    QVariant returnValue = browseAction->invoke();
    if (returnValue.type() != QVariant::Map) {
        return false;
    }

    // Find & invoke the sendurl action
    Action *sendUrlAction = mApplication->actionRegistry()->find("sendurl");
    return sendUrlAction->invoke({
        { "device", returnValue.toMap().value("device") },
        { "enumerator", returnValue.toMap().value("enumerator") },
        { "url", url }
    });
}
