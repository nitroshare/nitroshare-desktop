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

#include "deviceproxymodel.h"

int DeviceProxyModel::columnCount(const QModelIndex &) const
{
    return ColumnCount;
}

QVariant DeviceProxyModel::data(const QModelIndex &proxyIndex, int role) const
{
    if (!proxyIndex.isValid() || proxyIndex.row() < 0 || proxyIndex.row() >= rowCount()) {
        return QVariant();
    }

    Device *device = sourceData(proxyIndex, Qt::UserRole).value<Device*>();

    switch (role) {
    case Qt::DisplayRole:
        switch (proxyIndex.column()) {
        case NameColumn:
            return device->name();
        }
        break;
    }

    return sourceData(proxyIndex, role);
}

QVariant DeviceProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole) {
        return QVariant();
    }
    switch (section) {
    case NameColumn:
        return tr("Name");
    }
    return QVariant();
}
