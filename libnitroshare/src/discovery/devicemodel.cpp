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

Device *DeviceModelPrivate::findDevice(const QString &uuid)
{
    Device *device = nullptr;
    foreach (Device *d, devices) {
        if (d->uuid() == uuid) {
            device = d;
        }
    }
    return device;
}

void DeviceModelPrivate::onDeviceUpdated(const QString &uuid, const QStringList &addresses, const QVariantMap &properties)
{
    // If no matching device was found, create one
    Device *device = findDevice(uuid);
    bool created = false;
    if (!device) {
        device = new Device(uuid, this);
        devices.append(device);
        created = true;
    }

    // Add the addresses for the device
    foreach (QString address, addresses) {
        device->addAddress(address);
    }

    // Update the properties for the device
    auto i = properties.constBegin();
    while (i != properties.constEnd()) {
        if (i.key() == "name") {
            device->setName(i.value().toString());
        } else if (i.key() == "port") {
            device->setPort(i.value().toInt());
        }
        ++i;
    }

    // Emit the appropriate signal(s)
    if (created) {
        q->beginInsertRows(QModelIndex(), devices.count(), devices.count());
        devices.append(device);
        q->endInsertRows();
    } else {
        QModelIndex index = q->index(devices.indexOf(device, 0));
        emit q->dataChanged(index, index);
    }
}

void DeviceModelPrivate::onDeviceRemoved(const QString &uuid)
{
    // Attempt to find the device
    Device *device = findDevice(uuid);

    // If found, remove it from the list
    if (device) {
        int index = devices.indexOf(device);

        q->beginRemoveRows(QModelIndex(), index, index);
        devices.removeAt(index);
        q->endRemoveRows();

        delete device;
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
        return device->uuid();
    case NameRole:
        return device->name();
    case AddressesRole:
        return device->addresses();
    case PortRole:
        return device->port();
    }

    return QVariant();
}

QVariant DeviceModel::headerData(int, Qt::Orientation, int) const
{
    return QVariant();
}

QHash<int, QByteArray> DeviceModel::roleNames() const
{
    return {
        { UuidRole, "uuid" },
        { NameRole, "name" },
        { AddressesRole, "addresses" },
        { PortRole, "port" }
    };
}
