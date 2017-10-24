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

Plugin::Plugin(Application *application, const QString &filename, QObject *parent)
    : QObject(parent),
      d(new PluginPrivate(this, application, filename))
{
}

bool Plugin::isLoaded() const
{
    return d->loader.isLoaded() && d->iplugin;
}

bool Plugin::isInitialized() const
{
    return d->initialized;
}

QString Plugin::name() const
{
    d->metadata.value("Name").toString();
}

QString Plugin::title() const
{
    d->metadata.value("Title").toString();
}

QString Plugin::vendor() const
{
    d->metadata.value("Vendor").toString();
}

QString Plugin::version() const
{
    d->metadata.value("Version").toString();
}

QString Plugin::description() const
{
    d->metadata.value("Description").toString();
}

QStringList Plugin::dependencies() const
{
    d->metadata.value("Dependencies").toString();
}

bool Plugin::load()
{
    if (!d->loader.isLoaded()) {
        if (!d->loader.load()) {
            return false;
        }
        d->metadata = d->loader.metaData().value("MetaData").toObject();
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

void Plugin::initialize()
{
    if (d->loader.isLoaded() && !d->initialized) {
        d->iplugin->initialize(d->application);
        d->initialized = true;
    }
}

void Plugin::cleanup()
{
    if (d->initialized) {
        d->iplugin->cleanup(d->application);
        d->initialized = false;
    }
}
