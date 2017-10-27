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

#include <nitroshare/setting.h>

#include "setting_p.h"

const QString Setting::TypeKey = "type";
const QString Setting::NameKey = "name";
const QString Setting::TitleKey = "title";
const QString Setting::DefaultValueKey = "defaultValue";

SettingPrivate::SettingPrivate(QObject *parent, const QVariantMap &properties)
    : QObject(parent),
      properties(properties)
{
}

Setting::Setting(const QVariantMap &properties, QObject *parent)
    : QObject(parent),
      d(new SettingPrivate(this, properties))
{
}

Setting::Type Setting::type() const
{
    return d->properties.value(TypeKey).value<Type>();
}

QString Setting::name() const
{
    return d->properties.value(NameKey).toString();
}

QString Setting::title() const
{
    return d->properties.value(TitleKey).toString();
}

QVariant Setting::defaultValue() const
{
    return d->properties.value(DefaultValueKey);
}
