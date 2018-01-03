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

#include <QDir>
#include <QLibrary>

#include <nitroshare/application.h>
#include <nitroshare/iplugin.h>
#include <nitroshare/logger.h>
#include <nitroshare/message.h>
#include <nitroshare/plugin.h>
#include <nitroshare/pluginmodel.h>
#include <nitroshare/settingsregistry.h>

#include "plugin_p.h"
#include "pluginmodel_p.h"

const QString MessageTag = "pluginmodel";

PluginModelPrivate::PluginModelPrivate(QObject *parent, Application *application)
    : QObject(parent),
      application(application)
{
}

PluginModel::PluginModel(Application *application, QObject *parent)
    : QAbstractListModel(parent),
      d(new PluginModelPrivate(this, application))
{
}

PluginModel::~PluginModel()
{
    unloadAll();
    qDeleteAll(d->plugins);
}

void PluginModel::addToBlacklist(const QStringList &names)
{
    d->blacklist.append(names);
}

bool PluginModel::add(Plugin *plugin)
{
    // The plugin cannot be added unless it is loaded
    if (!plugin->d->load()) {
        delete plugin;
        return false;
    }

    // Add the plugin to the model
    beginInsertRows(QModelIndex(), d->plugins.count(), d->plugins.count());
    d->plugins.append(plugin);
    endInsertRows();

    return true;
}

void PluginModel::loadPluginsFromDirectories(const QStringList &directories)
{
    d->application->logger()->log(new Message(
        Message::Info,
        MessageTag,
        QString("loading plugins from %1").arg(directories.join(";"))
    ));

    // Load all of the plugins in the directories
    QList<Plugin*> newPlugins;
    foreach (const QString &directory, directories) {
        QDir dir(directory);
        foreach (QString filename, dir.entryList(QDir::Files)) {
            filename = dir.absoluteFilePath(filename);

            // If the file is a library, attempt to add it to the model
            if (QLibrary::isLibrary(filename)) {
                Plugin *plugin = new Plugin(filename);
                if (add(plugin)) {
                    newPlugins.append(plugin);
                }
            }
        }
    }

    // Attempt to initialize the new plugins
    foreach (Plugin *plugin, newPlugins) {
        if (!load(plugin)) {
            d->application->logger()->log(new Message(
                Message::Error,
                MessageTag,
                QString("unable to initialize %1").arg(plugin->name())
            ));
        }
    }
}

Plugin *PluginModel::find(const QString &name) const
{
    foreach (Plugin *plugin, d->plugins) {
        if (plugin->name() == name) {
            return plugin;
        }
    }
    return nullptr;
}

bool PluginModel::load(Plugin *plugin)
{
    if (!plugin->d->load()) {
        return false;
    }
    if (!plugin->d->initialized) {

        // Refuse to initialize blacklisted plugins
        QStringList blacklist = d->application->settingsRegistry()->value(
            Application::PluginBlacklistSettingName
        ).toStringList();
        if (blacklist.contains(plugin->name()) || d->blacklist.contains(plugin->name())) {
            return false;
        }

        // For each dependency, check if the plugin exists and if so, attempt
        // to initialize it
        QList<Plugin*> dependentPlugins;
        foreach (const QString &dependency, plugin->d->dependencies) {
            if (dependency == "ui") {
                if (d->application->isUiEnabled()) {
                    continue;
                } else {
                    return false;
                }
            }
            Plugin *dependentPlugin = find(dependency);
            if (!dependentPlugin || !load(dependentPlugin)) {
                return false;
            }
            dependentPlugins.append(dependentPlugin);
        }

        // Retrieve the IPlugin interface from the plugin
        IPlugin *iplugin = qobject_cast<IPlugin*>(plugin->d->loader.instance());
        if (!iplugin) {
            return false;
        }
        iplugin->initialize(d->application);
        plugin->d->initialized = true;

        // Emit the signal indicating the state of the plugin has changed
        QModelIndex index = createIndex(d->plugins.indexOf(plugin), 0);
        emit dataChanged(index, index);

        // Prevent the dependencies from cleanup until this one is cleaned up
        foreach (Plugin *dependentPlugin, dependentPlugins) {
            dependentPlugin->d->children.append(plugin);
        }
    }
    return true;
}

bool PluginModel::unload(Plugin *plugin)
{
    if (plugin->d->initialized) {

        // Unload all children first
        foreach (Plugin *childPlugin, plugin->d->children) {
            if (!unload(childPlugin)) {
                return false;
            }
        }

        // Clean up this plugin
        IPlugin *iplugin = qobject_cast<IPlugin*>(plugin->d->loader.instance());
        iplugin->cleanup(d->application);
        plugin->d->initialized = false;

        // Emit the signal indicating the state of the plugin has changed
        QModelIndex index = createIndex(d->plugins.indexOf(plugin), 0);
        emit dataChanged(index, index);

        // Remove this plugin from its dependencies
        foreach (const QString &dependency, plugin->d->dependencies) {
            if (dependency != "ui") {
               find(dependency)->d->children.removeOne(plugin);
            }
        }
    }
    if (plugin->d->loader.isLoaded()) {
        return plugin->d->loader.unload();
    }
    return true;
}

void PluginModel::unloadAll()
{
    foreach (Plugin *plugin, d->plugins) {
        unload(plugin);
    }
}

int PluginModel::rowCount(const QModelIndex &) const
{
    return d->plugins.count();
}

QVariant PluginModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 ||
            index.row() >= d->plugins.count() || role != Qt::UserRole) {
        return QVariant();
    }
    return QVariant::fromValue(d->plugins.at(index.row()));
}
