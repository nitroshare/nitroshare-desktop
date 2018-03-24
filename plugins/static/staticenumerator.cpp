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

#include <QSet>
#include <QTimer>

#include <nitroshare/application.h>
#include <nitroshare/settingsregistry.h>

#include "staticdevice.h"
#include "staticenumerator.h"

const QString StaticCategoryName = "static";
const QString StaticDevicesName = "StaticDevices";

StaticEnumerator::StaticEnumerator(Application *application)
    : mApplication(application),
      mStaticCategory({
          { Category::NameKey, StaticCategoryName },
          { Category::TitleKey, tr("Static") }
      }),
      mStaticDevices({
          { Setting::TypeKey, Setting::StringList },
          { Setting::NameKey, StaticDevicesName },
          { Setting::TitleKey, tr("Static devices") },
          { Setting::CategoryKey, StaticCategoryName }
      })
{
    mApplication->settingsRegistry()->addCategory(&mStaticCategory);
    mApplication->settingsRegistry()->addSetting(&mStaticDevices);

    connect(mApplication->settingsRegistry(), &SettingsRegistry::settingsChanged, this, &StaticEnumerator::onSettingsChanged);

    // Load the initial settings
    QTimer::singleShot(0, [this]() {
        onSettingsChanged({StaticDevicesName});
    });
}

StaticEnumerator::~StaticEnumerator()
{
    mApplication->settingsRegistry()->removeSetting(&mStaticDevices);
    mApplication->settingsRegistry()->removeCategory(&mStaticCategory);

    qDeleteAll(mDevices);
}

QString StaticEnumerator::name() const
{
    return "static";
}

void StaticEnumerator::onSettingsChanged(const QStringList &keys)
{
    if (keys.contains(StaticDevicesName)) {
        auto addresses = mApplication->settingsRegistry()->value(StaticDevicesName).toStringList().toSet();

        // Remove existing addresses not present in the new list
        for (auto i = mDevices.begin(); i != mDevices.end();) {
            if (!addresses.contains(i.key())) {
                Device *device = i.value();

                // Indicate that a device was removed
                emit deviceRemoved(device);

                // Remove the device from the map and destroy it
                i = mDevices.erase(i);
                delete device;

            } else {
                ++i;
            }
        }

        // Add the new addresses
        foreach (auto address, addresses) {
            if (!mDevices.contains(address)) {

                // Create a new device instance and add it to the map
                StaticDevice *device = new StaticDevice(address);
                mDevices.insert(address, device);

                // Indicate that a device was added
                emit deviceAdded(device);
            }
        }
    }
}
