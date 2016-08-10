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

#include <nitroshare/devicemodel.h>

#include "devicemodel_p.h"

DeviceModelPrivate::DeviceModelPrivate(QObject *parent)
    : QObject(parent)
{
}

DeviceModelPrivate::~DeviceModelPrivate()
{
    qDeleteAll(devices);
}

void DeviceModelPrivate::onDeviceAdded(Device *device)
{
}

void DeviceModelPrivate::onDeviceRemoved(const QString &uuid)
{
}

void DeviceModel::addEnumerator(DeviceEnumerator *enumerator)
{
    connect(enumerator, &DeviceEnumerator::deviceAdded, d, &DeviceModelPrivate::onDeviceAdded);
    connect(enumerator, &DeviceEnumerator::deviceRemoved, d, &DeviceModelPrivate::onDeviceRemoved);
}

int DeviceModel::rowCount(const QModelIndex &) const
{
    return d->devices.count();
}

QVariant DeviceModel::data(const QModelIndex &index, int role) const
{
    return QVariant();
}

QVariant DeviceModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return QVariant();
}

QHash<int, QByteArray> DeviceModel::roleNames() const
{
    return QHash<int, QByteArray>({});
}

DeviceModel::DeviceModel(QObject *parent)
    : QAbstractListModel(parent)
    , d(new DeviceModelPrivate(parent))
{
}