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

#include <QGuiApplication>
#include <QPalette>

#include <nitroshare/pluginmodel.h>

#include "pluginproxymodel.h"

void PluginProxyModel::setSourceModel(QAbstractItemModel *sourceModel)
{
    QAbstractProxyModel::setSourceModel(sourceModel);

    connect(sourceModel, &QAbstractItemModel::dataChanged, this,
            [this](const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &) {
        emit dataChanged(
            createIndex(topLeft.row(), 0),
            createIndex(bottomRight.row(), ColumnCount - 1)
        );
    });
}

QModelIndex PluginProxyModel::index(int row, int column, const QModelIndex &) const
{
    return createIndex(row, column);
}

QModelIndex PluginProxyModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    return createIndex(sourceIndex.row(), 0);
}

QModelIndex PluginProxyModel::mapToSource(const QModelIndex &proxyIndex) const
{
    return sourceModel()->index(proxyIndex.row(), 0);
}

QModelIndex PluginProxyModel::parent(const QModelIndex &) const
{
    return QModelIndex();
}

int PluginProxyModel::rowCount(const QModelIndex &) const
{
    return sourceModel()->rowCount();
}

int PluginProxyModel::columnCount(const QModelIndex &) const
{
    return ColumnCount;
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
            if (sourceRole(proxyIndex, PluginModel::IsInitializedRole).toBool()) {
                return tr("Initialized");
            } else if (sourceRole(proxyIndex, PluginModel::IsLoadedRole).toBool()) {
                return tr("Loaded");
            } else {
                return tr("Unloaded");
            }
            break;
        }
        break;
    case Qt::TextAlignmentRole:
        switch (proxyIndex.column()) {
        case VersionColumn:
        case StatusColumn:
            return Qt::AlignCenter;
        }
        break;
    case Qt::ForegroundRole:
        if (!sourceRole(proxyIndex, PluginModel::IsInitializedRole).toBool()) {
            return QGuiApplication::palette().color(QPalette::Disabled, QPalette::WindowText);
        }
        break;
    }

    return sourceRole(proxyIndex, role);
}

QVariant PluginProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole) {
        return QVariant();
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

QVariant PluginProxyModel::sourceRole(const QModelIndex &proxyIndex, int role) const
{
    return sourceModel()->data(mapToSource(proxyIndex), role);
}
