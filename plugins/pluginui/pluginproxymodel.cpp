/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Nathan Osman
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

#include <nitroshare/plugin.h>
#include <nitroshare/pluginmodel.h>

#include "pluginproxymodel.h"

int PluginProxyModel::columnCount(const QModelIndex &) const
{
    return ColumnCount;
}

QVariant PluginProxyModel::data(const QModelIndex &proxyIndex, int role) const
{
    if (!proxyIndex.isValid() || proxyIndex.row() < 0 || proxyIndex.row() >= rowCount()) {
        return QVariant();
    }

    Plugin *plugin = sourceData(proxyIndex, Qt::UserRole).value<Plugin*>();

    switch (role) {
    case Qt::DisplayRole:
        switch (proxyIndex.column()) {
        case NameColumn:
            return plugin->name();
        case TitleColumn:
            return plugin->title();
        case VendorColumn:
            return plugin->vendor();
        case VersionColumn:
            return plugin->version();
        case StatusColumn:
            if (plugin->isLoaded()) {
                return tr("Loaded");
            } else {
                return tr("Unloaded");
            }
            break;
        case DescriptionColumn:
            return plugin->description();
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
        if (!plugin->isLoaded()) {
            return QGuiApplication::palette().color(QPalette::Disabled, QPalette::WindowText);
        }
        break;
    }

    return sourceData(proxyIndex, role);
}

QVariant PluginProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole) {
        return QVariant();
    }
    switch (section) {
    case NameColumn:
        return tr("Name");
    case TitleColumn:
        return tr("Title");
    case VendorColumn:
        return tr("Vendor");
    case VersionColumn:
        return tr("Version");
    case StatusColumn:
        return tr("Status");
    case DescriptionColumn:
        return tr("Description");
    }
    return QVariant();
}
