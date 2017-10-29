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

#include <QSet>

#include <nitroshare/device.h>

#include "device_p.h"

const QString Device::UuidKey = "uuid";
const QString Device::NameKey = "name";
const QString Device::AddressesKey = "addresses";
const QString Device::PortKey = "port";

DevicePrivate::DevicePrivate(QObject *parent)
    : QObject(parent)
{
}

void DevicePrivate::update(DeviceEnumerator *deviceEnumerator, const QVariantMap &newProperties)
{
    // Loop through each of the properties, adding them to the device's map or
    // (in the case of addresses), adding them to the device=>address map
    for (auto i = newProperties.constBegin(); i != newProperties.constEnd(); ++i) {
        if (i.key() == Device::AddressesKey) {
            auto newAddresses = i.value().toStringList().toSet();
            auto j = addresses.find(deviceEnumerator);
            if (j != addresses.end()) {
                j.value().unite(newAddresses);
            } else {
                addresses.insert(deviceEnumerator, newAddresses);
            }
        } else {
            properties.insert(i.key(), i.value());
        }
    }
}

void DevicePrivate::remove(DeviceEnumerator *deviceEnumerator)
{
    addresses.remove(deviceEnumerator);
}

Device::Device(QObject *parent)
    : QObject(parent),
      d(new DevicePrivate(this))
{
}

QString Device::uuid() const
{
    return d->properties.value(UuidKey).toString();
}

QString Device::name() const
{
    return d->properties.value(NameKey).toString();
}

QStringList Device::addresses() const
{
    QSet<QString> uniqueAddresses;
    for (auto i = d->addresses.constBegin(); i != d->addresses.constEnd(); ++i) {
        uniqueAddresses.unite(i.value());
    }
    return uniqueAddresses.toList();
}

quint16 Device::port() const
{
    return d->properties.value(PortKey).toUInt();
}
