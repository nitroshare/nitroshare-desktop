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

DeviceModelPrivate::DeviceModelPrivate(DeviceModel *deviceModel)
    : QObject(deviceModel),
      q(deviceModel)
{
    connect(&timer, &QTimer::timeout, deviceModel, &DeviceModel::update);
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
    QString version = object.value("version").toString();
    QString name = object.value("name").toString();
    QString operatingSystem = object.value("operating_system").toString();
    quint16 port = object.value("port").toInt();

    // Ensure that the minimum set of values were provided and are valid
    if(uuid.isEmpty() || uuid == Settings::get<QString>(Settings::DeviceUUID) ||
            version != PROJECT_VERSION || !port) {
        return;
    }

    // Attempt to find a device by UUID, creating it if one does not exist
    Device *device = q->find(uuid);
    bool created = false;

    if(!device) {
        device = new Device(uuid);
        created = true;
    }

    // Update the values
    if(!name.isEmpty()) {
        device->setName(name);
    }

    if(!operatingSystem.isEmpty()) {
        device->setOperatingSystem(operatingSystem);
    }

    device->update(address, port);

    // If a device was created, add it to the list
    if(created) {
        q->beginInsertRows(QModelIndex(), devices.count(), devices.count());
        devices.append(device);
        q->endInsertRows();

        emit q->deviceAdded(device);
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
    if(index.row() < d->devices.count() && index.column() < DeviceModelPrivate::ColumnCount) {
        Device *device = d->devices.at(index.row());

        switch(role) {
        case Qt::DisplayRole:
            switch(index.column()) {
            case DeviceModelPrivate::ColumnName:
                return device->name();
            case DeviceModelPrivate::ColumnOperatingSystem:
                return device->operatingSystem();
            }
        case Qt::DecorationRole:
            if(index.column() == DeviceModelPrivate::ColumnName) {
                return QVariant::fromValue(QIcon(":/img/desktop.svg"));
            }
            break;
        case Qt::UserRole:
            return QVariant::fromValue(device);
        }
    }

    return QVariant();
}

QVariant DeviceModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(section < DeviceModelPrivate::ColumnCount && orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch(section) {
        case DeviceModelPrivate::ColumnName:
            return tr("Device Name");
        case DeviceModelPrivate::ColumnOperatingSystem:
            return tr("Operating System");
        }
    }

    return QVariant();
}

Device* DeviceModel::find(const QString &uuid)
{
    for(QList<Device *>::const_iterator i(d->devices.constBegin());
            i != d->devices.constEnd(); ++i) {
        if((*i)->uuid() == uuid) {
            return *i;
        }
    }

    return nullptr;
}

void DeviceModel::update()
{
    // Iterate over the list in reverse to preserve indices when items are removed
    for(int i = d->devices.count() - 1; i >= 0; --i) {
        Device *device = d->devices.at(i);

        if(device->expired()) {
            beginRemoveRows(QModelIndex(), i, i);
            d->devices.removeAt(i);
            endRemoveRows();

            emit deviceRemoved(device);
            delete device;
        }
    }
}
