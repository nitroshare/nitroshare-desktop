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
#include <nitroshare/deviceenumerator.h>
#include <nitroshare/devicemodel.h>

#include "device_p.h"
#include "devicemodel_p.h"

DeviceModelPrivate::DeviceModelPrivate(DeviceModel *model)
    : QObject(model),
      q(model)
{
}

DeviceModelPrivate::~DeviceModelPrivate()
{
    qDeleteAll(devices);
}

void DeviceModelPrivate::onDeviceUpdated(const QString &uuid, const QVariantMap &properties)
{
    DeviceEnumerator *enumerator = qobject_cast<DeviceEnumerator*>(sender());

    // Attempt to find an existing device, creating one if it does not exist
    Device *device = q->find(uuid);
    bool newDevice = static_cast<bool>(device);
    if (newDevice) {
        device = new Device;
    }

    // TODO: emit signal for change

    device->d->update(enumerator, properties);

    if (newDevice) {
        q->beginInsertRows(QModelIndex(), devices.count(), devices.count());
        devices.append(device);
        q->endInsertRows();
    }
}

void DeviceModelPrivate::onDeviceRemoved(const QString &uuid)
{
    // TODO
}

DeviceModel::DeviceModel(QObject *parent)
    : QAbstractListModel(parent),
      d(new DeviceModelPrivate(this))
{
}

void DeviceModel::addDeviceEnumerator(DeviceEnumerator *enumerator)
{
    connect(enumerator, &DeviceEnumerator::deviceUpdated, d, &DeviceModelPrivate::onDeviceUpdated);
    connect(enumerator, &DeviceEnumerator::deviceRemoved, d, &DeviceModelPrivate::onDeviceRemoved);
}

void DeviceModel::removeDeviceEnumerator(DeviceEnumerator *enumerator)
{
    disconnect(enumerator, &DeviceEnumerator::deviceUpdated, d, &DeviceModelPrivate::onDeviceUpdated);
    disconnect(enumerator, &DeviceEnumerator::deviceRemoved, d, &DeviceModelPrivate::onDeviceRemoved);

    // Remove anything associated with the enumerator from each device
    foreach (Device *device, d->devices) {
        device->d->remove(enumerator);
    }
}

Device *DeviceModel::find(const QString &uuid) const
{
    foreach (Device *device, d->devices) {
        if (device->uuid() == uuid) {
            return device;
        }
    }
    return nullptr;
}

int DeviceModel::rowCount(const QModelIndex &) const
{
    return d->devices.count();
}

QVariant DeviceModel::data(const QModelIndex &index, int role) const
{
    // Ensure the index points to a valid row
    if (!index.isValid() || index.row() < 0 ||
            index.row() >= d->devices.count() || role != Qt::UserRole) {
        return QVariant();
    }
    return QVariant::fromValue(d->devices.at(index.row()));
}
