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

#include <QDir>
#include <QLibrary>

#include <nitroshare/plugin.h>
#include <nitroshare/pluginmodel.h>

#include "pluginmodel_p.h"

PluginModelPrivate::PluginModelPrivate(QObject *parent, Application *application)
    : QObject(parent),
      application(application)
{
}

PluginModelPrivate::~PluginModelPrivate()
{
    qDeleteAll(plugins);
}

PluginModel::PluginModel(Application *application, QObject *parent)
    : QAbstractListModel(parent),
      d(new PluginModelPrivate(this, application))
{
}

void PluginModel::addPlugins(const QString &directory)
{
    // Attempt to load each of the plugins
    foreach (QString filename, QDir(directory).entryList(QDir::Files)) {

        // First verify that the file is indeed a library (by extension)
        if (!QLibrary::isLibrary(filename)) {
            continue;
        }

        // Attempt to load the plugin
        QPluginLoader *loader = new QPluginLoader(filename);
        if (loader->isLoaded()) {

            // The plugin must derive from the Plugin class
            Plugin *plugin = qobject_cast<Plugin*>(loader->instance());
            if (plugin) {

                // Add the plugin to the list
                beginInsertRows(QModelIndex(), d->plugins.count(), d->plugins.count());
                d->plugins.append(loader);
                endInsertRows();

                // Initialize the plugin
                plugin->init(d->application);

                continue;
            }
        }

        // The plugin failed to load, free the memory
        delete loader;
    }
}

int PluginModel::rowCount(const QModelIndex &parent) const
{
    return d->plugins.count();
}

QVariant PluginModel::data(const QModelIndex &index, int role) const
{
    // Ensure the index points to a valid row
    if (!index.isValid() || index.row() < 0 || index.row() >= d->plugins.count()) {
        return QVariant();
    }

    QPluginLoader *loader = d->plugins.at(index.row());

    switch(role) {
    case FilenameRole:
        return loader->fileName();
    }

    return QVariant();
}

QHash<int, QByteArray> PluginModel::roleNames() const
{
    return {
        { FilenameRole, "filename" }
    };
}
