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

#include <nitroshare/application.h>
#include <nitroshare/logger.h>
#include <nitroshare/message.h>
#include <nitroshare/plugin.h>
#include <nitroshare/pluginmodel.h>

#include "pluginmodel_p.h"

const QString LoggerTag = "pluginmodel";

PluginModelPrivate::PluginModelPrivate(PluginModel *model, Application *application)
    : QObject(model),
      q(model),
      application(application)
{
}

PluginModelPrivate::~PluginModelPrivate()
{
    q->unloadAll();
}

PluginModel::PluginModel(Application *application, QObject *parent)
    : QAbstractListModel(parent),
      d(new PluginModelPrivate(this, application))
{
    //...
}

Plugin *PluginModel::findPlugin(const QString &name)
{
    foreach (Plugin *plugin, d->plugins) {
        if (plugin->name() == name) {
            return plugin;
        }
    }
    return nullptr;
}

void PluginModel::loadPluginsFromDirectories(const QStringList &directories)
{
    d->application->logger()->log(new Message(
        Message::Info,
        LoggerTag,
        QString("loading plugins from %1").arg(directories.join(", "))
    ));

    //...
}

void PluginModel::unloadPlugin(const QString &name)
{
    //...
}

void PluginModel::unloadAll()
{
    //...
}

int PluginModel::rowCount(const QModelIndex &parent) const
{
    return d->plugins.count();
}

QVariant PluginModel::data(const QModelIndex &index, int role) const
{
    return QVariant();
}

QHash<int, QByteArray> PluginModel::roleNames() const
{
    return {};
}
