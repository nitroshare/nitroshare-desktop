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

#include <QJsonValue>

#include <nitroshare/application.h>
#include <nitroshare/iplugin.h>
#include <nitroshare/plugin.h>
#include <nitroshare/pluginmodel.h>

#include "plugin_p.h"

PluginPrivate::PluginPrivate(Plugin *plugin, const QString &filename)
    : QObject(plugin),
      q(plugin),
      loader(filename),
      initialized(false)
{
}

QStringList PluginPrivate::arrayToList(const QJsonArray &array)
{
    QStringList list;
    foreach (const QJsonValue &value, array) {
        list.append(value.toString());
    }
    return list;
}

bool PluginPrivate::load()
{
    if (!loader.isLoaded()) {
        if (!loader.load()) {
            return false;
        }
        metadata = loader.metaData().value("MetaData").toObject();
        dependencies = arrayToList(metadata.value("Dependencies").toArray());
    }
    return true;
}

bool PluginPrivate::unload(Application *application)
{
    if (initialized) {

        // Unload all children first
        foreach (Plugin *plugin, children) {
            if (!plugin->d->unload(application)) {
                return false;
            }
        }

        // Clean up this plugin
        qobject_cast<IPlugin*>(loader.instance())->cleanup(application);
        initialized = false;

        // Remove this plugin from its dependencies
        foreach (const QString &dependency, dependencies) {
            if (dependency != "ui") {
                application->pluginModel()->find(dependency)->d->children.removeOne(q);
            }
        }
    }
    if (loader.isLoaded()) {
        return loader.unload();
    }
    return true;
}

bool PluginPrivate::initialize(Application *application)
{
    if (!initialized) {

        // For each dependency, check if the plugin exists and if so, attempt
        // to initialize it
        QList<Plugin*> dependentPlugins;
        foreach (const QString &dependency, dependencies) {
            if (dependency == "ui") {
                if (application->isUiEnabled()) {
                    continue;
                } else {
                    return false;
                }
            }
            Plugin *dependentPlugin = application->pluginModel()->find(dependency);
            if (!dependentPlugin || !dependentPlugin->d->initialize(application)) {
                return false;
            }
            dependentPlugins.append(dependentPlugin);
        }

        // Retrieve the IPlugin interface from the plugin
        IPlugin *iplugin = qobject_cast<IPlugin*>(loader.instance());
        if (!iplugin) {
            return false;
        }
        iplugin->initialize(application);
        initialized = true;

        // Prevent the dependencies from cleanup until this one is cleaned up
        foreach (Plugin *dependentPlugin, dependentPlugins) {
            dependentPlugin->d->children.append(q);
        }
    }
    return true;
}

Plugin::Plugin(const QString &filename, QObject *parent)
    : QObject(parent),
      d(new PluginPrivate(this, filename))
{
}

QString Plugin::name() const
{
    return d->metadata.value("Name").toString();
}

QString Plugin::title() const
{
    return d->metadata.value("Title").toString();
}

QString Plugin::vendor() const
{
    return d->metadata.value("Vendor").toString();
}

QString Plugin::version() const
{
    return d->metadata.value("Version").toString();
}

QString Plugin::description() const
{
    return d->metadata.value("Description").toString();
}

bool Plugin::isLoaded() const
{
    return d->loader.isLoaded();
}
