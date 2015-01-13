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
#include <QSettings>
#include <QUuid>

#include "settings.h"

struct Setting
{
    QString name;
    QVariant (*initialize)();
};

#define DEFINE_SETTING(x,y) \
    { \
        Settings::x, { \
            #x, \
            []() -> QVariant y \
        } \
    }

QMap<Settings::Key, Setting> keys {
    DEFINE_SETTING(DeviceTimeout, { return 30 * 1000; }),
    DEFINE_SETTING(InterfaceMonitorInterval, { return 10 * 1000; }),
    DEFINE_SETTING(MulticastAddress, { return "ffx8::64"; }),
    DEFINE_SETTING(MulticastPort, { return 40816; }),
    DEFINE_SETTING(Name, { return QHostInfo::localHostName(); }),
    DEFINE_SETTING(PingInterval, { return 5 * 1000; }),
    DEFINE_SETTING(TransferPort, { return 40818; }),
    DEFINE_SETTING(UUID, { return QUuid::createUuid(); })
};

Q_GLOBAL_STATIC(Settings, settings)

QVariant Settings::loadValue(Key key)
{
    Setting setting = keys.value(key);

    if(!settings->contains(setting.name)) {
        Settings::storeValue(key, setting.initialize(), true);
    }

    return settings->value(setting.name);
}

void Settings::storeValue(Key key, const QVariant &value, bool initializing)
{
    settings->setValue(keys.value(key).name, value);

    if(!initializing) {
        emit settings->settingChanged(key);
    }
}

Settings * Settings::instance()
{
    return settings;
}
