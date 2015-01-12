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

#define DEFINE_SETTING(x,y) \
    const QString Settings::x::y = #x "/" #y;

DEFINE_SETTING(Discovery, InterfaceMonitorInterval)
DEFINE_SETTING(Discovery, MulticastAddress)
DEFINE_SETTING(Discovery, MulticastPort)
DEFINE_SETTING(Discovery, Name)
DEFINE_SETTING(Discovery, UUID)

QMap<QString, QVariant (*)()> defaults {
    {
        Settings::Discovery::InterfaceMonitorInterval,
        []() -> QVariant { return 10 * 1000; }
    },
    {
        Settings::Discovery::MulticastAddress,
        []() -> QVariant { return "ffx8::64"; }
    },
    {
        Settings::Discovery::MulticastPort,
        []() -> QVariant { return 40816; }
    },
    {
        Settings::Discovery::Name,
        []() -> QVariant { return QHostInfo::localHostName(); }
    },
    {
        Settings::Discovery::UUID,
        []() -> QVariant { return QUuid::createUuid(); }
    }
};

Q_GLOBAL_STATIC(QSettings, settings)

QVariant Settings::get(QString key)
{
    if(!settings->contains(key) && defaults.contains(key))
        Settings::set(key, defaults.value(key)());
    return settings->value(key);
}

void Settings::set(QString key, const QVariant &value)
{
    settings->setValue(key, value);
}
