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
#include <QVBoxLayout>

#include <nitroshare/application.h>
#include <nitroshare/category.h>
#include <nitroshare/setting.h>
#include <nitroshare/settingsregistry.h>

#include "booleansettingwidget.h"
#include "pathsettingwidget.h"
#include "settingsdialog.h"
#include "stringsettingwidget.h"

SettingsDialog::SettingsDialog(Application *application)
    : mApplication(application),
      mTabWidget(new QTabWidget),
      mMiscWidget(nullptr)
{
    setWindowTitle(tr("Settings"));

    connect(mApplication->settingsRegistry(), &SettingsRegistry::settingAdded, this, &SettingsDialog::onSettingAdded);
    connect(mApplication->settingsRegistry(), &SettingsRegistry::settingRemoved, this, &SettingsDialog::onSettingRemoved);

    // Add exisiting settings
    foreach (Setting *setting, mApplication->settingsRegistry()->settings()) {
        onSettingAdded(setting);
    }

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &SettingsDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &SettingsDialog::reject);

    QVBoxLayout *vboxLayout = new QVBoxLayout;
    vboxLayout->addWidget(mTabWidget);
    vboxLayout->addWidget(buttonBox);
    setLayout(vboxLayout);
}

void SettingsDialog::accept()
{
    // Set the value for all settings by retrieving the value from the widget
    mApplication->settingsRegistry()->begin();
    for (QHash<Setting*, SettingWidget*>::const_iterator i = mWidgets.constBegin();
            i != mWidgets.constEnd(); ++i) {
        mApplication->settingsRegistry()->setValue(
            i.key()->name(),
            i.value()->value()
        );
    }
    mApplication->settingsRegistry()->end();

    // Close the dialog
    QDialog::accept();
}

void SettingsDialog::onSettingAdded(Setting *setting)
{
    // Only add settings that are not hidden
    if (setting->isHidden()) {
        return;
    }

    // Attempt to create the setting's widget
    SettingWidget *widget = createWidget(setting);
    if (!widget) {
        return;
    }

    // Load the default value into the widget
    widget->setValue(mApplication->settingsRegistry()->value(setting->name()));

    // Add the widget to the appropriate parent widget
    QVBoxLayout *vboxLayout = qobject_cast<QVBoxLayout*>(getTab(setting)->layout());
    vboxLayout->insertWidget(vboxLayout->count() - 1, widget);
}

void SettingsDialog::onSettingRemoved(Setting *setting)
{
    // Unregister and remove the widget from its parent
    SettingWidget *widget = mWidgets.take(setting);
    widget->layout()->removeWidget(widget);

    // Use the parent widget to select the category name
    QString categoryName = mCategories.key(widget->parentWidget());

    // Destroy the widget
    delete widget;

    // Look for other settings in the same category
    foreach (Setting *otherSetting, mWidgets.keys()) {
        if (otherSetting->category() == categoryName) {
            return;
        }
    }

    // If none exist, remove the tab and delete its widget
    QWidget *tabWidget = mCategories.take(categoryName);
    mTabWidget->removeTab(mTabWidget->indexOf(tabWidget));
    delete tabWidget;
}

QWidget *SettingsDialog::getTab(Setting *setting)
{
    // First check to see if the tab exists
    QWidget *widget = mCategories.value(setting->category());
    if (widget) {
        return widget;
    }

    QString name;
    QString title;

    // Attempt to lookup the category to obtain the correct information; if one
    // does not exist, try the miscellaneous category
    Category *category = mApplication->settingsRegistry()->findCategory(setting->category());
    if (!category) {
        widget = mCategories.value("misc");
        if (widget) {
            return widget;
        }
    }

    // If that failed, prepare to create a new tab
    if (category) {
        name = category->name();
        title = category->title();
    } else {
        name = "misc";
        title = tr("Miscellaneous");
    }

    // Create the widget
    QVBoxLayout *vboxLayout = new QVBoxLayout;
    vboxLayout->addStretch(1);
    widget = new QWidget;
    widget->setLayout(vboxLayout);

    // Add the widget to the tab widget and register it
    mTabWidget->addTab(widget, title);
    mCategories.insert(name, widget);

    return widget;
}

SettingWidget *SettingsDialog::createWidget(Setting *setting)
{
    SettingWidget *widget = nullptr;

    // TODO: handle integer settings differently

    // Create a widget of the appropriate type
    switch (setting->type()) {
    case Setting::String:
    case Setting::Integer:
        widget = new StringSettingWidget(setting);
        break;
    case Setting::Boolean:
        widget = new BooleanSettingWidget(setting);
        break;
    case Setting::FilePath:
    case Setting::DirectoryPath:
        widget = new PathSettingWidget(setting);
        break;
    default:
        return nullptr;
    }

    // Store the newly created widget in the appropriate location
    mWidgets.insert(setting, widget);

    return widget;
}
