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
    foreach (Plugin *plugin, d->plugins) {
        plugin->d->unload(d->application);
    }
}

void PluginModel::addToBlacklist(const QStringList &names)
{
    d->blacklist.append(names);
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

            // Ensure that the file is a library
            if (!QLibrary::isLibrary(filename)) {
                continue;
            }

            // Attempt to load the file as a plugin
            Plugin *plugin = new Plugin(filename);
            if (!plugin->d->load()) {
                delete plugin;
                continue;
            }

            // Refuse to load blacklisted plugins
            if (d->blacklist.contains(plugin->name())) {
                delete plugin;
                continue;
            }

            // Insert the plugin into the model
            beginInsertRows(QModelIndex(), d->plugins.count(), d->plugins.count());
            d->plugins.append(plugin);
            endInsertRows();

            // Indicate when plugin state changes
            connect(plugin, &Plugin::isLoadedChanged, [this, plugin]() {
                QModelIndex idx = index(d->plugins.indexOf(plugin), 0);
                emit dataChanged(idx, idx, { Qt::UserRole });
            });

            newPlugins.append(plugin);
        }
    }

    // Attempt to initialize the new plugins
    foreach (Plugin *plugin, newPlugins) {
        if (!plugin->d->initialize(d->application)) {
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
    if (!plugin->d->load() || !plugin->d->initialize(d->application)) {
        return false;
    }
    return true;
}

bool PluginModel::unload(Plugin *plugin)
{
    if (!plugin->d->unload(d->application)) {
        return false;
    }
    return true;
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
