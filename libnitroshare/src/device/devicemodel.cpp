/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Nathan Osman
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

void DeviceModelPrivate::removeDevice(Device *device)
{
    int index = devices.indexOf(device);
    if (index != -1) {
        q->beginRemoveRows(QModelIndex(), index, index);
        devices.removeAt(index);
        q->endRemoveRows();

        disconnect(device, &Device::nameChanged, this, &DeviceModelPrivate::onDeviceUpdated);
    }
}

void DeviceModelPrivate::onDeviceAdded(Device *device)
{
    DeviceEnumerator *enumerator = qobject_cast<DeviceEnumerator*>(sender());
    device->d->deviceEnumeratorName = enumerator->name();

    q->beginInsertRows(QModelIndex(), devices.count(), devices.count());
    devices.append(device);
    q->endInsertRows();

    connect(device, &Device::nameChanged, this, &DeviceModelPrivate::onDeviceUpdated);
}

void DeviceModelPrivate::onDeviceRemoved(Device *device)
{
    removeDevice(device);
}

void DeviceModelPrivate::onDeviceUpdated()
{
    auto index = q->index(devices.indexOf(qobject_cast<Device*>(sender())), 0);
    emit q->dataChanged(index, index);
}

DeviceModel::DeviceModel(QObject *parent)
    : QAbstractListModel(parent),
      d(new DeviceModelPrivate(this))
{
}

void DeviceModel::addDeviceEnumerator(DeviceEnumerator *enumerator)
{
    connect(enumerator, &DeviceEnumerator::deviceAdded, d, &DeviceModelPrivate::onDeviceAdded);
    connect(enumerator, &DeviceEnumerator::deviceRemoved, d, &DeviceModelPrivate::onDeviceRemoved);
}

void DeviceModel::removeDeviceEnumerator(DeviceEnumerator *enumerator)
{
    disconnect(enumerator, &DeviceEnumerator::deviceAdded, d, &DeviceModelPrivate::onDeviceAdded);
    disconnect(enumerator, &DeviceEnumerator::deviceRemoved, d, &DeviceModelPrivate::onDeviceRemoved);

    // Remove all items that belong to the enumerator
    foreach (Device *device, d->devices) {
        if (device->deviceEnumeratorName() == enumerator->name()) {
            d->removeDevice(device);
        }
    }
}

Device *DeviceModel::findDevice(const QString &uuid, const QString &enumeratorName)
{
    foreach (Device *device, d->devices) {
        if (device->uuid() == uuid && device->deviceEnumeratorName() == enumeratorName) {
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
    if (!index.isValid() || index.row() < 0 ||
            index.row() >= d->devices.count() || role != Qt::UserRole) {
        return QVariant();
    }
    return QVariant::fromValue(d->devices.at(index.row()));
}
