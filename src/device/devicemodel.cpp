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

#include "config.h"
#include "devicemodel.h"

DeviceModel::DeviceModel()
{
    connect(&mTimeoutTimer, &QTimer::timeout, this, &DeviceModel::checkTimeouts);
    connect(&mDeviceListener, &DeviceListener::pingReceived, this, &DeviceModel::processPing);
    connect(Settings::instance(), &Settings::settingChanged, this, &DeviceModel::settingChanged);

    reload();
}

void DeviceModel::start()
{
    mDeviceListener.start();
}

int DeviceModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : mDevices.count();
}

int DeviceModel::columnCount(const QModelIndex &parent) const
{
    // The model displays name and operating system
    return parent.isValid() ? 0 : 2;
}

QVariant DeviceModel::data(const QModelIndex &index, int role) const
{
    DevicePointer device(mDevices.at(index.row()));

    switch(role) {
    case Qt::DisplayRole:
        switch(index.column()) {
        case 0: return device->name();
        case 1: return device->operatingSystem();
        }
    case Qt::UserRole:
        return QVariant::fromValue(device);
    }

    return QVariant();
}

QVariant DeviceModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch(section) {
        case 0: return tr("Device Name");
        case 1: return tr("Operating System");
        }
    }

    return QVariant();
}

void DeviceModel::checkTimeouts()
{
    // Iterate over the list in reverse to preserve indices when items are removed
    for(int i = mDevices.count() - 1; i; --i) {
        if(mDevices.at(i)->timeoutReached()) {
            beginRemoveRows(QModelIndex(), i, i + 1);
            mDevices.removeAt(i);
            endRemoveRows();
        }
    }
}

void DeviceModel::processPing(const QJsonObject &object, const QHostAddress &address)
{
    if(object.contains("uuid") && object.contains("version")) {
        QString uuid(object.value("uuid").toString());
        QString version(object.value("version").toString());

        if(uuid != Settings::get<QString>(Settings::DeviceUUID) && version == NITROSHARE_VERSION) {

            // Iterate over the list of devices, searching for a UUID match
            for(int i = 0; i < mDevices.count(); ++i) {
                if(mDevices.at(i)->uuid() == uuid) {
                    mDevices.at(i)->update(object, address);
                    return;
                }
            }

            // No match was found, create a new device...
            DevicePointer device(new Device(uuid));
            device->update(object, address);

            // ...and insert it into the list
            beginInsertRows(QModelIndex(), mDevices.count(), mDevices.count() + 1);
            mDevices.append(device);
            endInsertRows();
        }
    }
}

void DeviceModel::settingChanged(Settings::Key key)
{
    if(key == Settings::BroadcastTimeout) {
        mTimeoutTimer.stop();
        reload();
        mTimeoutTimer.start();
    }
}

void DeviceModel::reload()
{
    mTimeoutTimer.setInterval(Settings::get<int>(Settings::BroadcastTimeout));
}
