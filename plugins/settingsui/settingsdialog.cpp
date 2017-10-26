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

#include <nitroshare/application.h>
#include <nitroshare/setting.h>
#include <nitroshare/settingsregistry.h>

#include "settingsdialog.h"
#include "stringsettingwidget.h"

SettingsDialog::SettingsDialog(Application *application)
    : mApplication(application),
      mLayout(new QVBoxLayout),
      mSpacer(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding))
{
    setWindowTitle(tr("Settings"));

    connect(mApplication->settingsRegistry(), &SettingsRegistry::settingAdded, this, &SettingsDialog::onSettingAdded);
    connect(mApplication->settingsRegistry(), &SettingsRegistry::settingRemoved, this, &SettingsDialog::onSettingRemoved);

    // Add exisiting settings
    foreach (Setting *setting, mApplication->settingsRegistry()->settings()) {
        onSettingAdded(setting);
    }

    mLayout->addItem(mSpacer);
    setLayout(mLayout);
}

void SettingsDialog::onSettingAdded(Setting *setting)
{
    SettingWidget *widget = nullptr;

    // Create the widget of the appropriate type
    switch (setting->type()) {
    case Setting::String:
        widget = new StringSettingWidget(setting);
        break;
    }

    if (widget) {

        // Set the initial value
        widget->setValue(mApplication->settingsRegistry()->value(setting->name()));

        // Add the widget
        mLayout->insertWidget(mWidgets.count(), widget);
        mWidgets.insert(setting, widget);
    }
}

void SettingsDialog::onSettingRemoved(Setting *setting)
{
    SettingWidget *widget = mWidgets.take(setting);
    mLayout->removeWidget(widget);
    delete widget;
}
