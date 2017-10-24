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

#include <iterator>

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
    while (plugins.count()) {
        unloadPlugin(plugins.take(plugins.firstKey()));
    }
    qDeleteAll(plugins);
}

PluginModel::PluginModel(Application *application, QObject *parent)
    : QAbstractListModel(parent),
      d(new PluginModelPrivate(this, application))
{
}

Plugin *PluginModelPrivate::findPlugin(const QString &name)
{
    foreach (Plugin *plugin, plugins) {
        if (plugin->name() == name) {
            return plugin;
        }
    }
    return nullptr;
}

void PluginModelPrivate::keyChanged(const QString &name)
{
    int index = std::distance(plugins.constBegin(), plugins.constFind(name));
    emit q->dataChanged(q->index(index), q->index(index));
}

bool PluginModelPrivate::loadPlugin(Plugin *plugin)
{
    foreach (const QString &name, plugin->dependencies()) {
        Plugin *dependentPlugin = plugins.value(name);
        if (!dependentPlugin || !loadPlugin(dependentPlugin)) {
            return false;
        }
    }
    if (!plugin->load()) {
        return false;
    }
    keyChanged(plugin->name());
    return true;
}

void PluginModelPrivate::unloadPlugin(Plugin *plugin)
{
    foreach (Plugin *childPlugin, plugin->children()) {
        unloadPlugin(childPlugin);
    }
    plugin->unload();
    keyChanged(plugin->name());
}

void PluginModelPrivate::initializePlugin(Plugin *plugin)
{
    foreach (const QString &name, plugin->dependencies()) {
        initializePlugin(plugins.value(name));
    }
    plugin->initialize();
    keyChanged(plugin->name());
}

void PluginModelPrivate::cleanupPlugin(Plugin *plugin)
{
    foreach (Plugin *childPlugin, plugin->children()) {
        cleanupPlugin(childPlugin);
    }
    plugin->cleanup();
    keyChanged(plugin->name());
}

void PluginModel::loadPluginsFromDirectories(const QStringList &directories)
{
    // TODO
}

bool PluginModel::loadPlugin(const QString &name)
{
    Plugin *plugin = d->plugins.value(name);
    if (!plugin) {
        return false;
    }
    return d->loadPlugin(plugin);
}

bool PluginModel::unloadPlugin(const QString &name)
{
    Plugin *plugin = d->findPlugin(name);
    if (!plugin) {
        return false;
    }
    d->unloadPlugin(plugin);
    return true;
}

bool PluginModel::initializePlugin(const QString &name)
{
    Plugin *plugin = d->findPlugin(name);
    if (!plugin) {
        return false;
    }
    d->initializePlugin(plugin);
    return true;
}

bool PluginModel::cleanupPlugin(const QString &name)
{
    Plugin *plugin = d->findPlugin(name);
    if (!plugin) {
        return false;
    }
    d->cleanupPlugin(plugin);
    return true;
}

int PluginModel::rowCount(const QModelIndex &parent) const
{
    return d->plugins.count();
}

QVariant PluginModel::data(const QModelIndex &index, int role) const
{
    // TODO

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
        { InitializedRole, "initialized" }
    };
}
