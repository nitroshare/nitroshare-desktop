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

#include "settingsaction.h"
#include "settingsdialog.h"

SettingsAction::SettingsAction(Application *application)
    : mApplication(application),
      mDialog(nullptr)
{
}

SettingsAction::~SettingsAction()
{
    if (mDialog) {
        delete mDialog;
    }
}

QString SettingsAction::name() const
{
    return "settingsui";
}

bool SettingsAction::menu() const
{
    return true;
}

QString SettingsAction::label() const
{
    return tr("Settings...");
}

QVariant SettingsAction::invoke(const QVariantMap &)
{
    // Create the dialog if it does not exist
    if (!mDialog) {
        mDialog = new SettingsDialog(mApplication);
        connect(mDialog, &SettingsDialog::finished, [this]() {
            mDialog->deleteLater();
            mDialog = nullptr;
        });
    }

    // Show the dialog either way
    mDialog->show();

    return true;
}
