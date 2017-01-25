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

#include <QJsonArray>
#include <QJsonObject>

#include <nitroshare/iplugin.h>
#include <nitroshare/plugin.h>

#include "plugin_p.h"

const QString NameKey = "Name";
const QString TitleKey = "Title";
const QString VendorKey = "Vendor";
const QString VersionKey = "Version";
const QString DescriptionKey = "Description";
const QString DependenciesKey = "Dependencies";

PluginPrivate::PluginPrivate(QObject *parent, Application *application, const QString &filename)
    : QObject(parent),
      application(application),
      loader(filename),
      iplugin(nullptr)
{
}

PluginPrivate::~PluginPrivate()
{
    if (iplugin) {
        iplugin->cleanup(application);
    }
}

Plugin::Plugin(Application *application, const QString &filename, QObject *parent)
    : QObject(parent),
      d(new PluginPrivate(this, application, filename))
{
}

bool Plugin::load()
{
    if (d->loader.load()) {
        d->metadata = d->loader.metaData().value("MetaData").toObject();
    }
    return d->loader.isLoaded();
}

bool Plugin::initialize()
{
    d->iplugin = qobject_cast<IPlugin*>(d->loader.instance());
    if (d->iplugin) {
        d->iplugin->initialize(d->application);
    }
    return d->iplugin;
}

QString Plugin::name() const
{
    return d->metadata.value(NameKey).toString();
}

QString Plugin::title() const
{
    return d->metadata.value(TitleKey).toString();
}

QString Plugin::vendor() const
{
    return d->metadata.value(VendorKey).toString();
}

QString Plugin::version() const
{
    return d->metadata.value(VersionKey).toString();
}

QString Plugin::description() const
{
    return d->metadata.value(DescriptionKey).toString();
}

QStringList Plugin::dependencies() const
{
    QJsonArray array = d->metadata.value(DependenciesKey).toArray();
    QStringList list;
    foreach (QJsonValue value, array) {
        list.append(value.toString());
    }
    return list;
}
