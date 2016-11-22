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

#include <QModelIndex>

#include <nitroshare/devicemodel.h>

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

int DeviceModelPrivate::findDevice(const QString &uuid)
{
    // Search for an existing device with the UUID
    for (int i = 0; i < devices.count(); ++i) {
        if (devices.at(i)->uuid == uuid) {
            return i;
        }
    }

    // Return an invalid index if the file was not found
    return -1;
}

bool DeviceModelPrivate::removeEnumerator(QObject *enumerator, int index)
{
    Device *device = devices.at(index);

    // Determine if the enumerator provided any addresses and then remove it
    // from the address map for the device
    bool empty = device->addressMap.value(enumerator).isEmpty();
    bool removed = device->addressMap.remove(enumerator);

    // If no addresses remain, remove the device and delete it
    if (device->addressMap.isEmpty()) {

        // Indicate to the model that the device is gone
        q->beginRemoveRows(QModelIndex(), index, index);
        devices.removeAt(index);
        q->endRemoveRows();

        delete device;
        return true;
    }

    // If addresses were removed,
    if (!empty && removed) {
        emit q->dataChanged(q->index(index), q->index(index));
    }

    return false;
}

void DeviceModelPrivate::onDeviceUpdated(const QString &uuid, const QVariantMap &properties)
{
    Device *device = nullptr;
    int index = findDevice(uuid);
    if (index != -1) {
        device = devices.at(index);
    }

    // If the device does not exist, create it
    if (!device) {
        device = new Device;
        device->uuid = uuid;
    }

    // Update the properties for the device, taking note of anything that changed
    bool changed = false;
    auto i = properties.constBegin();
    while (i != properties.constEnd()) {
        if (i.key() == DeviceEnumerator::AddressesKey) {
            QStringList addresses = i.value().toStringList();
            qSort(addresses.begin(), addresses.end());
            if (device->addressMap.value(sender()) != addresses) {
                device->addressMap.insert(sender(), addresses);
                changed = true;
            }
        } else if (device->properties.value(i.key()) != i.value()) {
            device->properties.insert(i.key(), i.value());
            changed = true;
        }
        ++i;
    }

    // Emit the appropriate signal(s)
    if (index == -1) {
        q->beginInsertRows(QModelIndex(), devices.count(), devices.count());
        devices.append(device);
        q->endInsertRows();
    } else if (changed) {
        emit q->dataChanged(q->index(index), q->index(index));
    }
}

void DeviceModelPrivate::onDeviceRemoved(const QString &uuid)
{
    int index = findDevice(uuid);
    if (index != -1) {
        removeEnumerator(sender(), index);
    }
}

DeviceModel::DeviceModel(QObject *parent)
    : QAbstractListModel(parent),
      d(new DeviceModelPrivate(this))
{
}

void DeviceModel::addEnumerator(DeviceEnumerator *enumerator)
{
    connect(enumerator, &DeviceEnumerator::deviceUpdated, d, &DeviceModelPrivate::onDeviceUpdated);
    connect(enumerator, &DeviceEnumerator::deviceRemoved, d, &DeviceModelPrivate::onDeviceRemoved);
}

void DeviceModel::removeEnumerator(DeviceEnumerator *enumerator)
{
    disconnect(enumerator, &DeviceEnumerator::deviceUpdated, d, &DeviceModelPrivate::onDeviceUpdated);
    disconnect(enumerator, &DeviceEnumerator::deviceRemoved, d, &DeviceModelPrivate::onDeviceRemoved);

    for (int i = 0; i < d->devices.count(); ++i) {
        if (d->removeEnumerator(enumerator, i)) {
            --i;
        }
    }
}

int DeviceModel::rowCount(const QModelIndex &) const
{
    return d->devices.count();
}

QVariant DeviceModel::data(const QModelIndex &index, int role) const
{
    // Ensure the index points to a valid row
    if (!index.isValid() || index.row() < 0 || index.row() >= d->devices.count()) {
        return QVariant();
    }

    Device *device = d->devices.at(index.row());

    switch (role) {
    case UuidRole:
        return device->uuid;
    case NameRole:
    case VersionRole:
    case OperatingSystemRole:
    case PortRole:
        return device->properties.value(roleNames().value(role));
    case AddressesRole:
        return device->addresses();
    }

    return QVariant();
}

QHash<int, QByteArray> DeviceModel::roleNames() const
{
    return {
        { UuidRole, DeviceEnumerator::UuidKey.toUtf8() },
        { NameRole, DeviceEnumerator::NameKey.toUtf8() },
        { VersionRole, DeviceEnumerator::VersionKey.toUtf8() },
        { OperatingSystemRole, DeviceEnumerator::OperatingSystemKey.toUtf8() },
        { AddressesRole, DeviceEnumerator::AddressesKey.toUtf8() },
        { PortRole, DeviceEnumerator::PortKey.toUtf8() }
    };
}
