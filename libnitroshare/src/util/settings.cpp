/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Nathan Osman
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

#include <nitroshare/settings.h>

#include "settings_p.h"

SettingsPrivate::SettingsPrivate(QObject *parent, QSettings *settings)
    : QObject(parent),
      settings(settings),
      addToPending(false)
{
}

Settings::Settings(QSettings *settings, QObject *parent)
    : QObject(parent),
      d(new SettingsPrivate(this, settings))
{
}

QVariant Settings::get(const QString &key, QVariant (*defaultValue)())
{
    if (d->settings->contains(key)) {
        return d->settings->value(key);
    } else {
        QVariant value = defaultValue();
        d->settings->setValue(key, value);
        return value;
    }
}

void Settings::set(const QString &key, const QVariant &value)
{
    if (!d->settings->contains(key) || d->settings->value(key) != value) {
        d->settings->setValue(key, value);

        // Don't emit the signal if a group of values are being set
        if (d->addToPending) {
            d->pendingKeys.append(key);
        } else {
            emit settingsChanged({ key });
        }
    }
}

void Settings::begin()
{
    d->addToPending = true;
}

void Settings::end()
{
    if (d->pendingKeys.count()) {
        emit settingsChanged(d->pendingKeys);
    }

    d->addToPending = false;
    d->pendingKeys.clear();
}
