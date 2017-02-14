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

#include <nitroshare/settings.h>

#include "settings_p.h"

const QString Settings::DefaultKey = "default";

SettingsPrivate::SettingsPrivate(QObject *parent, QSettings *settings)
    : QObject(parent),
      settings(settings),
      suppress(false)
{
}

Settings::Settings(QSettings *settings, QObject *parent)
    : QObject(parent),
      d(new SettingsPrivate(this, settings))
{
}

void Settings::addSetting(const QString &key, const QVariantMap &metadata)
{
    // If the setting doesn't have a value, apply the default
    if (!d->settings->contains(key)) {
        d->settings->setValue(key, metadata.value(DefaultKey));
    }

    d->metadataMap.insert(key, metadata);

    if (d->suppress) {
        d->suppressedAdditions.append(key);
    } else {
        emit settingsAdded(QStringList{key});
    }
}

void Settings::removeSetting(const QString &key)
{
    d->metadataMap.remove(key);
    if (d->suppress) {
        d->suppressedRemovals.append(key);
    } else {
        emit settingsRemoved(QStringList{key});
    }
}

QVariantMap Settings::settings() const
{
    return d->metadataMap;
}

QVariant Settings::value(const QString &key) const
{
    return d->settings->value(key);
}

void Settings::setValue(const QString &key, const QVariant &value)
{
    if (d->settings->value(key) != value) {
        d->settings->setValue(key, value);
        if (d->suppress) {
            d->suppressedChanges.append(key);
        } else {
            emit settingsChanged(QStringList{key});
        }
    }
}

void Settings::begin()
{
    d->suppress = true;
}

void Settings::end()
{
    if (d->suppressedAdditions.length()) {
        emit settingsAdded(d->suppressedAdditions);
        d->suppressedAdditions.clear();
    }

    if (d->suppressedRemovals.length()) {
        emit settingsRemoved(d->suppressedRemovals);
        d->suppressedRemovals.clear();
    }

    if (d->suppressedChanges.length()) {
        emit settingsChanged(d->suppressedChanges);
        d->suppressedChanges.clear();
    }

    d->suppress = false;
}

