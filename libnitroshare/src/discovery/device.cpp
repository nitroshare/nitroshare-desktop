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

#include <nitroshare/device.h>

#include "device_p.h"

const QString Device::UuidKey = "uuid";
const QString Device::NameKey = "name";

DevicePrivate::DevicePrivate(QObject *parent, const QString &uuid)
    : QObject(parent),
      uuid(uuid)
{
}

bool DevicePrivate::update(DeviceEnumerator *deviceEnumerator, const QVariantMap &newProperties)
{
    auto i = properties.find(deviceEnumerator);
    if (i != properties.end()) {
        if (i.value() == newProperties) {
            return false;
        }
        i.value() = newProperties;
        return true;
    } else {
        properties.insert(deviceEnumerator, newProperties);
        return true;
    }
}

bool DevicePrivate::remove(DeviceEnumerator *deviceEnumerator)
{
    properties.remove(deviceEnumerator);
    return properties.count();
}

Device::Device(const QString &uuid)
    : d(new DevicePrivate(this, uuid))
{
}

QString Device::uuid() const
{
    return d->uuid;
}

