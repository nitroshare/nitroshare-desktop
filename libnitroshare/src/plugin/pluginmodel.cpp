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

#include <QDir>
#include <QLibrary>

#include <nitroshare/application.h>
#include <nitroshare/logger.h>
#include <nitroshare/message.h>
#include <nitroshare/plugin.h>
#include <nitroshare/pluginmodel.h>

#include "pluginmodel_p.h"

const QString MessageTag = "pluginmodel";

PluginModelPrivate::PluginModelPrivate(PluginModel *model, Application *application)
    : QObject(model),
      q(model),
      application(application)
{
}

PluginModelPrivate::~PluginModelPrivate()
{
    qDeleteAll(pluginList);
}

void PluginModelPrivate::emitChangeSignal(Plugin *plugin)
{
    QModelIndex index = q->index(pluginList.indexOf(plugin));
    emit q->dataChanged(index, index);
}

bool PluginModelPrivate::loadPlugin(Plugin *plugin)
{
    if (!plugin->load()) {
        return false;
    }
    emitChangeSignal(plugin);
    return true;
}

void PluginModelPrivate::unloadPlugin(Plugin *plugin)
{
    foreach (Plugin *childPlugin, plugin->children()) {
        unloadPlugin(childPlugin);
    }
    plugin->unload();
    emitChangeSignal(plugin);
}

bool PluginModelPrivate::initializePlugin(Plugin *plugin)
{
    if (pluginBlacklist.contains(plugin->name())) {
        return false;
    }
    foreach (const QString &dependency, plugin->dependencies()) {
        if (dependency == "ui") {
            continue;
        }
        Plugin *dependentPlugin = pluginHash.value(dependency);
        if (!dependentPlugin || !initializePlugin(dependentPlugin)) {
            return false;
        }
        dependentPlugin->addChild(plugin);
    }
    if (!plugin->initialize()) {
        return false;
    }
    emitChangeSignal(plugin);
    return true;
}

void PluginModelPrivate::cleanupPlugin(Plugin *plugin)
{
    foreach (Plugin *childPlugin, plugin->children()) {
        cleanupPlugin(childPlugin);
        plugin->removeChild(childPlugin);
    }
    plugin->cleanup();
    emitChangeSignal(plugin);
}

PluginModel::PluginModel(Application *application, QObject *parent)
    : QAbstractListModel(parent),
      d(new PluginModelPrivate(this, application))
{
}

PluginModel::~PluginModel()
{
    foreach (Plugin *plugin, d->pluginList) {
        d->cleanupPlugin(plugin);
    }
}

void PluginModel::addToBlacklist(const QStringList &names)
{
    d->pluginBlacklist.append(names);
}

void PluginModel::loadPluginsFromDirectories(const QStringList &directories)
{
    foreach (const QString &directory, directories) {
        d->application->logger()->log(new Message(
            Message::Info,
            MessageTag,
            QString("loading plugins from %1").arg(directory)
        ));
        QDir dir(directory);
        foreach (QString filename, dir.entryList(QDir::Files)) {
            filename = dir.absoluteFilePath(filename);
            if (!QLibrary::isLibrary(filename)) {
                continue;
            }
            Plugin *plugin = new Plugin(d->application, filename);
            if (!plugin->load()) {
                delete plugin;
                continue;
            }
            int newIndex = d->pluginList.count();
            beginInsertRows(QModelIndex(), newIndex, newIndex);
            d->pluginList.append(plugin);
            d->pluginHash.insert(plugin->name(), plugin);
            endInsertRows();
        }
    }
}

bool PluginModel::loadPlugin(const QString &name)
{
    Plugin *plugin = d->pluginHash.value(name);
    if (!plugin) {
        return false;
    }
    return d->loadPlugin(plugin);
}

bool PluginModel::unloadPlugin(const QString &name)
{
    Plugin *plugin = d->pluginHash.value(name);
    if (!plugin) {
        return false;
    }
    d->unloadPlugin(plugin);
    return true;
}

bool PluginModel::initializePlugin(const QString &name)
{
    Plugin *plugin = d->pluginHash.value(name);
    if (!plugin) {
        return false;
    }
    return d->initializePlugin(plugin);
}

void PluginModel::initializeAll()
{
    foreach (Plugin *plugin, d->pluginList) {
        d->initializePlugin(plugin);
    }
}

bool PluginModel::cleanupPlugin(const QString &name)
{
    Plugin *plugin = d->pluginHash.value(name);
    if (!plugin) {
        return false;
    }
    d->cleanupPlugin(plugin);
    return true;
}

int PluginModel::rowCount(const QModelIndex &) const
{
    return d->pluginList.count();
}

QVariant PluginModel::data(const QModelIndex &index, int role) const
{
    // Ensure the index points to a valid row
    if (!index.isValid() || index.row() < 0 || index.row() >= d->pluginList.count()) {
        return QVariant();
    }

    Plugin *plugin = d->pluginList.at(index.row());

    switch (role) {
    case NameRole:
        return plugin->name();
    case TitleRole:
        return plugin->title();
    case VendorRole:
        return plugin->vendor();
    case VersionRole:
        return plugin->version();
    case DescriptionRole:
        return plugin->dependencies();
    case IsLoadedRole:
        return plugin->isLoaded();
    case IsInitializedRole:
        return plugin->isInitialized();
    }

    return QVariant();
}

QHash<int, QByteArray> PluginModel::roleNames() const
{
    return {
        { NameRole, "name" },
        { TitleRole, "title" },
        { VendorRole, "vendor" },
        { VersionRole, "version" },
        { DescriptionRole, "description" },
        { DependenciesRole, "dependencies" },
        { IsLoadedRole, "isLoaded" },
        { IsInitializedRole, "isInitialized" }
    };
}
