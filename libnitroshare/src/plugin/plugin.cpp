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

#include <nitroshare/iplugin.h>
#include <nitroshare/plugin.h>

#include "plugin_p.h"

PluginPrivate::PluginPrivate(Plugin *plugin, Application *application, const QString &filename)
    : QObject(plugin),
      q(plugin),
      application(application),
      loader(filename),
      iplugin(nullptr),
      initialized(false)
{
}

PluginPrivate::~PluginPrivate()
{
    q->unload();
}

QStringList PluginPrivate::arrayToStringList(const QJsonArray &array)
{
    QStringList list;
    foreach (const QJsonValue &value, array) {
        list.append(value.toString());
    }
    return list;
}

Plugin::Plugin(Application *application, const QString &filename, QObject *parent)
    : QObject(parent),
      d(new PluginPrivate(this, application, filename))
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

QStringList Plugin::dependencies() const
{
    return d->dependencies;
}

bool Plugin::isLoaded() const
{
    return d->loader.isLoaded() && d->iplugin;
}

bool Plugin::isInitialized() const
{
    return d->initialized;
}

bool Plugin::load()
{
    if (!d->loader.isLoaded()) {
        if (!d->loader.load()) {
            return false;
        }
        d->metadata = d->loader.metaData().value("MetaData").toObject();
        d->dependencies = d->arrayToStringList(d->metadata.value("Dependencies").toArray());
        d->iplugin = qobject_cast<IPlugin*>(d->loader.instance());
    }
    return d->iplugin;
}

void Plugin::unload()
{
    cleanup();
    if (d->loader.isLoaded()) {
        d->loader.unload();
    }
}

bool Plugin::initialize()
{
    if (!d->initialized) {
        if (!d->loader.isLoaded()) {
            return false;
        }
        d->iplugin->initialize(d->application);
        d->initialized = true;
    }
    return true;
}

void Plugin::cleanup()
{
    if (d->initialized) {
        d->iplugin->cleanup(d->application);
        d->initialized = false;
    }
}

void Plugin::addChild(Plugin *plugin)
{
    d->children.insert(plugin);
}

void Plugin::removeChild(Plugin *plugin)
{
    d->children.remove(plugin);
}

QSet<Plugin*> Plugin::children() const
{
    return d->children;
}
