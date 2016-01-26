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

#include "settings.h"
#include "settings_p.h"

// For each individual key, store its name and an initialization function
struct KeyInfo
{
    QString name;
    QVariant (*initialize)();
};

// Macro used for generating the initialization list for a map
#define DEFINE_SETTING(x,y) \
    { \
        Settings::Key::x, { #x, []() -> QVariant y } \
    }

// Map keys to the appropriate KeyInfo instance
const QMap<Settings::Key, KeyInfo> KeyMap = {
    DEFINE_SETTING(ApplicationSplash, { return false; }),
    DEFINE_SETTING(BehaviorOverwrite, { return false; }),
    DEFINE_SETTING(BehaviorReceive, { return true; }),
    DEFINE_SETTING(BroadcastPort, { return 40816; }),
    DEFINE_SETTING(BroadcastInterval, { return 5 * Settings::Constant::Second; }),
    DEFINE_SETTING(BroadcastTimeout, { return 30 * Settings::Constant::Second; }),
    DEFINE_SETTING(DeviceUUID, { return QUuid::createUuid().toString(); }),
    DEFINE_SETTING(DeviceName, { return QHostInfo::localHostName(); }),
    DEFINE_SETTING(TransferPort, { return 40818; }),
    DEFINE_SETTING(TransferBuffer, { return 64 * Settings::Constant::KiB; }),
    DEFINE_SETTING(TransferDirectory, { return QStandardPaths::writableLocation(QStandardPaths::DesktopLocation); }),
};

SettingsPrivate::SettingsPrivate(Settings *settings)
    : q(settings)
{
}

QVariant SettingsPrivate::get(Settings::Key key, Flag flag)
{
    // Retrieve the name and initialization function for the key
    KeyInfo keyInfo = KeyMap.value(key);
    QVariant value = settings.value(keyInfo.name);

    // If no value was set and initialization is permited, initialize the setting
    if(value.isNull() && flag == Flag::Initialize) {
        value = keyInfo.initialize();
        set(key, value);
    }

    return value;
}

void SettingsPrivate::set(Settings::Key key, const QVariant &value, Flag flag)
{
    settings.setValue(KeyMap.value(key).name, value);

    // Take care of emitting the signal only if the appropriate flag was set
    if(flag == Flag::EmitSignal) {

        // If multiSet is set, add the setting to the list
        // Otherwise, emit the signal indicating a change
        if(multiSet) {
            multiSetKeys.append(key);
        } else {
            emit q->settingsChanged({key});
        }
    }
}

QVariant Settings::get(Key key)
{
    return d->get(key, SettingsPrivate::Flag::Initialize);
}

void Settings::beginSet()
{
    if(d->multiSet) {
        qWarning("beginSet() already invoked");
        return;
    }

    d->multiSet = true;
}

void Settings::set(Key key, const QVariant &value)
{
    // Retrieve the current value but don't initialize
    QVariant currentValue = d->get(key);

    // Set the value only if it has actually changed and emit the signal
    if(currentValue != value) {
        d->set(key, value, SettingsPrivate::Flag::EmitSignal);
    }
}

void Settings::endSet()
{
    if(!d->multiSet) {
        qWarning("beginSet() must be invoked before endSet()");
        return;
    }

    // If any keys were modified, emit the signal
    // with the list of keys and clear the list
    if(d->multiSetKeys.count()) {
        emit settingsChanged(d->multiSetKeys);
        d->multiSetKeys.clear();
    }

    d->multiSet = false;
}

void Settings::reset()
{
    beginSet();

    for(QMap<Settings::Key, KeyInfo>::const_iterator i = KeyMap.constBegin(); i != KeyMap.constEnd(); ++i) {
        set(i.key(), i.value().initialize());
    }

    endSet();
}

Settings *Settings::instance()
{
    // By using a static variable here, we allow the constructor and destructor
    // to be private, which prevents other instances and accidental deletion
    static Settings settings;

    return &settings;
}

Settings::Settings()
    : d(new SettingsPrivate(this))
{
}

Settings::~Settings()
{
    delete d;
}
