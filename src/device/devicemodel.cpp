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

#include <QIcon>

#include "../util/settings.h"
#include "config.h"
#include "devicemodel.h"
#include "devicemodel_p.h"

// Enable QHostAddress to be used in a QVariant
Q_DECLARE_METATYPE(QHostAddress)

DeviceModelPrivate::DeviceModelPrivate(DeviceModel *deviceModel)
    : QObject(deviceModel),
      q(deviceModel)
{
    connect(&timer, &QTimer::timeout, this, &DeviceModelPrivate::update);
    connect(&listener, &DeviceListener::pingReceived, this, &DeviceModelPrivate::processPing);
    connect(Settings::instance(), &Settings::settingChanged, this, &DeviceModelPrivate::settingChanged);

    reload();
}

DeviceModelPrivate::~DeviceModelPrivate()
{
    qDeleteAll(devices);
}

void DeviceModelPrivate::processPing(const QJsonObject &object, const QHostAddress &address)
{
    QString uuid = object.value("uuid").toString();

    // Ensure that the UUID is present and does not match the current
    // device since we will receive our own broadcast packets
    if(uuid.isEmpty() || uuid == Settings::get<QString>(Settings::DeviceUUID)) {
        return;
    }

    // Attempt to find an existing device with the UUID
    Device *device = nullptr;
    for(int i = 0; i < devices.count(); ++i) {
        if(devices.at(i)->uuid() == uuid) {
            device = devices.at(i);
        }
    }

    // Create a new device if one does not yet exist with the UUID
    bool created = false;
    if(!device) {
        device = new Device;
        created = true;
    }

    device->update(object.toVariantMap(), address);

    // If a new device was created, add it and watch for changes
    if(created) {
        q->beginInsertRows(QModelIndex(), devices.count(), devices.count());
        devices.append(device);
        q->endInsertRows();

        connect(device, &Device::dataChanged, [this, device]() {
            int index = devices.indexOf(device);
            emit q->dataChanged(q->index(index, 0), q->index(index, ColumnCount));
        });
    }
}

void DeviceModelPrivate::update()
{
    // Iterate over the list in reverse to preserve indices when items are removed
    for(int i = devices.count() - 1; i >= 0; --i) {
        Device *device = devices.at(i);

        if(device->expired()) {
            q->beginRemoveRows(QModelIndex(), i, i);
            devices.removeAt(i);
            q->endRemoveRows();

            delete device;
        }
    }
}

void DeviceModelPrivate::settingChanged(Settings::Key key)
{
    if(key == Settings::BroadcastTimeout) {
        reload();
    }
}

void DeviceModelPrivate::reload()
{
    timer.stop();
    timer.setInterval(Settings::get<int>(Settings::BroadcastTimeout));
    timer.start();
}

DeviceModel::DeviceModel()
    : d(new DeviceModelPrivate(this))
{
}

QModelIndex DeviceModel::index(int row, int column, const QModelIndex &parent) const
{
    if(row < 0 || column < 0 || row >= rowCount() || column >= columnCount() || parent.isValid()) {
        return QModelIndex();
    }

    return createIndex(row, column);
}

QModelIndex DeviceModel::parent(const QModelIndex &) const
{
    return QModelIndex();
}

int DeviceModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : d->devices.count();
}

int DeviceModel::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : DeviceModelPrivate::ColumnCount;
}

QVariant DeviceModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid() || index.model() != this) {
        return QVariant();
    }

    Device *device = d->devices.at(index.row());

    switch(role) {
    case Qt::DisplayRole:
        switch(index.column()) {
        case DeviceModelPrivate::ColumnName:
            return device->name();
        case DeviceModelPrivate::ColumnOperatingSystem:
            return device->operatingSystem();
        }
        break;
    case Qt::DecorationRole:
        if(index.column() == DeviceModelPrivate::ColumnName) {
            return QIcon(":/img/desktop.svg");
        }
        break;
    case UUIDRole:
        return device->uuid();
    case VersionRole:
        return device->version();
    case NameRole:
        return device->name();
    case OperatingSystemRole:
        return device->operatingSystem();
    case AddressRole:
        return QVariant::fromValue(device->address());
    case PortRole:
        return device->port();
    }

    return QVariant();
}

QVariant DeviceModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation != Qt::Horizontal || role != Qt::DisplayRole) {
        return QAbstractItemModel::headerData(section, orientation, role);
    }

    switch(section) {
    case DeviceModelPrivate::ColumnName:
        return tr("Device Name");
    case DeviceModelPrivate::ColumnOperatingSystem:
        return tr("Operating System");
    }

    return QVariant();
}

QHash<int, QByteArray> DeviceModel::roleNames() const
{
    return {
        {UUIDRole, "uuid"},
        {VersionRole, "version"},
        {NameRole, "name"},
        {OperatingSystemRole, "operating_system"},
        {AddressRole, "address"},
        {PortRole, "port"}
    };
}
