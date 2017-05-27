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

#include "config.h"

#ifdef qmdnsengine_FOUND
#  include <qmdnsengine/resolver.h>
#  include <qmdnsengine/service.h>
#endif

#include "../util/platform.h"
#include "devicemodel.h"
#include "devicemodel_p.h"

// Enable QHostAddress and OperatingSystem to be used in a QVariant
Q_DECLARE_METATYPE(QHostAddress)
Q_DECLARE_METATYPE(Platform::OperatingSystem)

DeviceModelPrivate::DeviceModelPrivate(DeviceModel *deviceModel)
    : QObject(deviceModel),
      q(deviceModel)
#ifdef qmdnsengine_FOUND
    , browser(&server, "_nitroshare._tcp.local.", &cache)
#endif
{
    connect(&timer, &QTimer::timeout, this, &DeviceModelPrivate::cleanup);
    connect(&listener, &DeviceListener::pingReceived, this, &DeviceModelPrivate::processPing);
    connect(Settings::instance(), &Settings::settingsChanged, this, &DeviceModelPrivate::onSettingsChanged);

#ifdef qmdnsengine_FOUND
    connect(&browser, &QMdnsEngine::Browser::serviceAdded, this, &DeviceModelPrivate::onServiceAddedOrUpdated);
    connect(&browser, &QMdnsEngine::Browser::serviceUpdated, this, &DeviceModelPrivate::onServiceAddedOrUpdated);
    connect(&browser, &QMdnsEngine::Browser::serviceRemoved, this, &DeviceModelPrivate::onServiceRemoved);
#endif

    onSettingsChanged();

    timer.start();
}

DeviceModelPrivate::~DeviceModelPrivate()
{
    qDeleteAll(devices);
}

void DeviceModelPrivate::update(const QString &uuid, const QString &name, Platform::OperatingSystem operatingSystem,
                                const QHostAddress &address, quint16 port, bool usesTls, bool expires)
{
    // Ensure that the UUID does not match this device
    // since we will receive our own broadcast packets
    if (uuid == Settings::instance()->get(Settings::Key::DeviceUUID).toString()) {
        return;
    }

    // Attempt to find an existing device with the UUID
    Device *device = nullptr;
    int row = 0;

    for (; row < devices.count(); ++row) {
        if (devices.at(row)->uuid() == uuid) {
            device = devices.at(row);
        }
    }

    // Create a new device if one does not yet exist with the UUID
    bool created = static_cast<bool>(!device);
    if (created) {
        device = new Device(uuid, expires);
    }

    // Update the device and check to see if anything important has changed
    bool changed = device->update(name, operatingSystem, address, port, usesTls);

    // Add the device if it was created, and otherwise, check to see
    // if something changed and emit the appropriate signal if so
    if (created) {
        q->beginInsertRows(QModelIndex(), devices.count(), devices.count());
        devices.append(device);
        q->endInsertRows();
    } else if (changed) {
        emit q->dataChanged(q->index(row, 0), q->index(row, DeviceModel::ColumnCount));
    }
}

void DeviceModelPrivate::processPing(const QString &uuid, const QString &name, Platform::OperatingSystem operatingSystem,
                                     const QHostAddress &address, quint16 port, bool usesTls)
{
    update(uuid, name, operatingSystem, address, port, usesTls, true);
}

void DeviceModelPrivate::cleanup()
{
    // Iterate over the list in reverse to preserve indices when items are removed
    for (int row = devices.count() - 1; row >= 0; --row) {
        Device *device = devices.at(row);

        // Remove the device if it has timed out
        if (device->hasTimedOut()) {
            q->beginRemoveRows(QModelIndex(), row, row);
            devices.removeAt(row);
            q->endRemoveRows();

            delete device;
        }
    }
}

void DeviceModelPrivate::onSettingsChanged(const QList<Settings::Key> &keys)
{
    if (keys.empty() || keys.contains(Settings::Key::BroadcastTimeout)) {
        timer.setInterval(Settings::instance()->get(Settings::Key::BroadcastTimeout).toInt());
    }
}

#ifdef qmdnsengine_FOUND

void DeviceModelPrivate::onServiceAddedOrUpdated(const QMdnsEngine::Service &service)
{
    // Extract the information from the TXT records if provided
    QString uuid = QString(service.attributes().value("uuid"));
    if (uuid.isEmpty()) {
        uuid = service.name();
    }
    QString os = QString(service.attributes().value("os"));
    Platform::OperatingSystem operatingSystem = Platform::operatingSystemForName(os);

    // Use a resolver to find an address for connecting
    QMdnsEngine::Resolver *resolver = new QMdnsEngine::Resolver(&server, service.hostname(), &cache, this);

    // Process the service once resolved
    connect(resolver, &QMdnsEngine::Resolver::resolved, [this, uuid, operatingSystem, service, resolver](const QHostAddress &address) {
        update(
            uuid,
            service.name(),
            operatingSystem,
            address,
            service.port(),
            false,
            false
        );
        resolver->deleteLater();
    });
}

void DeviceModelPrivate::onServiceRemoved(const QMdnsEngine::Service &service)
{
    QString uuid = QString(service.attributes().value("uuid"));
    if (uuid.isEmpty()) {
        uuid = service.name();
    }

    // Remove the device if found
    Device *device = nullptr;
    for (int row = devices.count() - 1; row >= 0; --row) {
        if (devices.at(row)->uuid() == uuid) {
            q->beginRemoveRows(QModelIndex(), row, row);
            devices.removeAt(row);
            q->endRemoveRows();

            delete device;
            break;
        }
    }
}

#endif // qmdnsengine_FOUND

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
    return parent.isValid() ? 0 : ColumnCount;
}

QVariant DeviceModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.model() != this) {
        return QVariant();
    }

    Device *device = d->devices.at(index.row());

    switch (role) {
    case Qt::DisplayRole:
        switch (index.column()) {
        case NameColumn:
            return device->name();
        case OperatingSystemColumn:
            return Platform::operatingSystemFriendlyName(device->operatingSystem());
        }
        break;
    case Qt::DecorationRole:
        switch (index.column()) {
        case NameColumn:
            if (device->usesTls()) {
                return QIcon(":/img/tls.svg");
            }
            break;
        case OperatingSystemColumn:
            switch (device->operatingSystem()) {
            case Platform::OperatingSystem::Android:
                return QIcon(":/img/mobile.svg");
            default:
                return QIcon(":/img/desktop.svg");
            }
            break;
        }
        break;
    case UUIDRole:
        return device->uuid();
    case NameRole:
        return device->name();
    case OperatingSystemRole:
        return QVariant::fromValue(device->operatingSystem());
    case AddressRole:
        return QVariant::fromValue(device->address());
    case PortRole:
        return device->port();
    case UsesTLSRole:
        return device->usesTls();
    }

    return QVariant();
}

QVariant DeviceModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole) {
        return QAbstractItemModel::headerData(section, orientation, role);
    }

    switch (section) {
    case NameColumn:
        return tr("Device Name");
    case OperatingSystemColumn:
        return tr("Operating System");
    }

    return QVariant();
}

QHash<int, QByteArray> DeviceModel::roleNames() const
{
    return {
        {UUIDRole, "uuid"},
        {NameRole, "name"},
        {OperatingSystemRole, "operating_system"},
        {AddressRole, "address"},
        {PortRole, "port"},
        {UsesTLSRole, "uses_tls"}
    };
}
