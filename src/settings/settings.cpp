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

#include <QHostInfo>
#include <QMap>
#include <QStandardPaths>
#include <QUuid>
#include <QVariant>

#include "settings.h"
#include "settings_p.h"

// Assign consecutive numbers to the constants
int Accumulator = 0;

#define DECLARE_SETTING(x) \
    const int Settings::x = Accumulator++;

DECLARE_SETTING(BroadcastInterval)
DECLARE_SETTING(BroadcastPort)
DECLARE_SETTING(BroadcastTimeout)
DECLARE_SETTING(DeviceName)
DECLARE_SETTING(DeviceUUID)
DECLARE_SETTING(TransferBuffer)
DECLARE_SETTING(TransferDirectory)
DECLARE_SETTING(TransferPort)
DECLARE_SETTING(UpdateInterval)

// For each setting, store its name and an initialization function
struct Setting
{
    QString name;
    QVariant (*initialize)();
};

// Macro used for inputting settings into the
#define DEFINE_SETTING(x,y) \
    { \
        Settings::x, { #x, []() -> QVariant y } \
    }

// Define all of the settings
QMap<int, Setting> Keys = {
    DEFINE_SETTING(BroadcastInterval, { return 5 * Settings::Second; }),
    DEFINE_SETTING(BroadcastPort, { return 40816; }),
    DEFINE_SETTING(BroadcastTimeout, { return 30 * Settings::Second; }),
    DEFINE_SETTING(DeviceName, { return QHostInfo::localHostName(); }),
    DEFINE_SETTING(DeviceUUID, { return QUuid::createUuid(); }),
    DEFINE_SETTING(TransferBuffer, { return 64 * Settings::KiB; }),
    DEFINE_SETTING(TransferDirectory, {
       return QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    }),
    DEFINE_SETTING(TransferPort, { return 40818; }),
    DEFINE_SETTING(UpdateInterval, { return 24 * Settings::Hour; })
};

// Global settings instance
Settings settings;

SettingsPrivate::SettingsPrivate(Settings *settings)
    : q(settings)
{
}

QVariant SettingsPrivate::get(int key, bool initialize)
{
    // Retrieve the setting for the key and retrieve its current value
    Setting setting = Keys.value(key);
    QVariant value = settings.value(setting.name);

    // If the value is NULL and we are allowed to initialize the variable, do so
    if(value.isNull() && initialize) {
        value = setting.initialize();
        set(key, value, true);
    }

    return value;
}

void SettingsPrivate::set(int key, const QVariant &value, bool initializing)
{
    // This method is only invoked if:
    // - the value was not set (initializing = true)
    // - the value was set but is different (initializing = false)

    // In either case, store the new value
    settings.setValue(Keys.value(key).name, value);

    // Only emit the changed signal if the key is not being initialized
    if(!initializing) {
        emit q->settingChanged(key);
    }
}

Settings::Settings(QObject *parent)
    : QObject(parent),
      d(new SettingsPrivate(this))
{
}

Settings::~Settings()
{
    delete d;
}

Settings *Settings::instance()
{
    return &settings;
}

void Settings::reset()
{
    for(QMap<int, Setting>::const_iterator i = Keys.constBegin(); i != Keys.constEnd(); ++i) {
        set(i.key(), i.value().initialize());
    }
}

QVariant Settings::get(int key)
{
    return instance()->d->get(key, true);
}

void Settings::set(int key, const QVariant &value)
{
    // Attempt to retrieve the current value
    QVariant currentValue = instance()->d->get(key, false);

    // Compare the current value to the new value
    // and if they differ, set the new value
    if(currentValue != value) {
        instance()->d->set(key, value, false);
    }
}
