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

#include <nitroshare/pluginmodel.h>

#include "pluginproxymodel.h"

PluginProxyModel::PluginProxyModel(QObject *parent)
    : QIdentityProxyModel(parent)
{
}

QModelIndex PluginProxyModel::index(int row, int column, const QModelIndex &parent) const
{
    if (column) {
        return createIndex(row, column);
    }
    return QIdentityProxyModel::index(row, column, parent);
}

int PluginProxyModel::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : ColumnCount;
}

QVariant PluginProxyModel::data(const QModelIndex &proxyIndex, int role) const
{
    if (!proxyIndex.isValid() || proxyIndex.row() < 0 || proxyIndex.row() >= rowCount()) {
        return QVariant();
    }

    switch (role) {
    case Qt::DisplayRole:
        switch (proxyIndex.column()) {
        case TitleColumn:
            role = PluginModel::TitleRole;
            break;
        case VendorColumn:
            role = PluginModel::VendorRole;
            break;
        case VersionColumn:
            role = PluginModel::VersionRole;
            break;
        case StatusColumn:
            if (QIdentityProxyModel::data(proxyIndex, PluginModel::IsInitializedRole).toBool()) {
                return tr("Initialized");
            } else if (QIdentityProxyModel::data(proxyIndex, PluginModel::IsLoadedRole).toBool()) {
                return tr("Loaded");
            } else {
                return tr("Unloaded");
            }
        }
        break;
    case Qt::TextAlignmentRole:
        switch (proxyIndex.column()) {
        case VersionColumn:
        case StatusColumn:
            return Qt::AlignCenter;
        }
        break;
    }

    return QIdentityProxyModel::data(proxyIndex, role);
}

QVariant PluginProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole) {
        return QIdentityProxyModel::headerData(section, orientation, role);
    }

    switch (section) {
    case TitleColumn:
        return tr("Title");
    case VendorColumn:
        return tr("Vendor");
    case VersionColumn:
        return tr("Version");
    case StatusColumn:
        return tr("Status");
    case ActionsColumn:
        return tr("Actions");
    }

    return QVariant();
}
