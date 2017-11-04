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

#include <nitroshare/application.h>
#include <nitroshare/category.h>
#include <nitroshare/setting.h>
#include <nitroshare/settingsregistry.h>

#include "booleansettingwidget.h"
#include "pathsettingwidget.h"
#include "settingsdialog.h"
#include "stringlistsettingwidget.h"
#include "stringsettingwidget.h"

SettingsDialog::SettingsDialog(Application *application)
    : mApplication(application),
      mTabWidget(new QTabWidget)
{
    setWindowTitle(tr("Settings"));
    resize(400, 400);

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
    mApplication->settingsRegistry()->begin();

    // Set the value for each setting in each tab
    for (auto i = mTabs.constBegin(); i != mTabs.constEnd(); ++i) {
        for (auto j = (*i).items.constBegin(); j != (*i).items.constEnd(); ++j) {
            mApplication->settingsRegistry()->setValue(
                (*j).setting->name(),
                (*j).widget->value()
            );
        }
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

    // Create the item
    createItem(setting);
}

void SettingsDialog::onSettingRemoved(Setting *setting)
{
    for (int i = 0; i < mTabs.count(); ++i) {
        Tab &tab = mTabs[i];
        for (int j = 0; j < tab.items.count(); ++j) {
            Item &item = tab.items[j];
            if (item.setting == setting) {

                // Remove the widget from the tab and destroy it
                tab.layout->removeWidget(item.widget);
                delete item.widget;

                // Remove the tab
                tab.items.removeAt(j);

                // If no more items remain in the tab, remove it too
                if (!tab.items.count()) {
                    mTabWidget->removeTab(i);
                    mTabs.removeAt(i);
                }

                return;
            }
        }
    }
}

SettingsDialog::Tab &SettingsDialog::getTab(Setting *setting)
{
    QString name;
    QString title;

    // Check to see if the category exists in the settings registry; if not,
    // the setting will be added to the "miscellaneous" tab
    Category *category = mApplication->settingsRegistry()->findCategory(setting->category());
    if (category) {
        name = category->name();
        title = category->title();
    } else {
        name = "misc";
        title = tr("Miscellaneous");
    }

    // Attempt to find an existing tab with the name
    for (auto i = mTabs.begin(); i != mTabs.end(); ++i) {
        if ((*i).name == name) {
            return *i;
        }
    }

    // No such tab exists; create the layout and widget

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addStretch(1);

    QWidget *widget = new QWidget;
    widget->setLayout(layout);

    // Create the new tab and register it
    mTabWidget->addTab(widget, title);
    mTabs.append({ name, widget, layout });

    // Return a reference to the new tab
    return mTabs[mTabs.count() - 1];
}

void SettingsDialog::createItem(Setting *setting)
{
    SettingWidget *widget = nullptr;

    // TODO: handle integer settings differently

    // Create a widget of the appropriate type
    switch (setting->type()) {
    case Setting::String:
    case Setting::Integer:
        widget = new StringSettingWidget(setting);
        break;
    case Setting::StringList:
        widget = new StringListSettingWidget(setting);
        break;
    case Setting::Boolean:
        widget = new BooleanSettingWidget(setting);
        break;
    case Setting::FilePath:
    case Setting::DirectoryPath:
        widget = new PathSettingWidget(setting);
        break;
    default:
        return;
    }

    // Load the default value
    widget->setValue(
        mApplication->settingsRegistry()->value(setting->name())
    );

    // Add the widget to the tab and register the new item
    Tab &tab = getTab(setting);
    tab.layout->insertWidget(tab.layout->count() - 1, widget);
    tab.items.append({setting, widget});
}
