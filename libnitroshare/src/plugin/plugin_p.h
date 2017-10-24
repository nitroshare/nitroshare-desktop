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

#ifndef LIBNITROSHARE_PLUGIN_P_H
#define LIBNITROSHARE_PLUGIN_P_H

#include <QJsonArray>
#include <QJsonObject>
#include <QList>
#include <QObject>
#include <QPluginLoader>
#include <QStringList>

class Application;
class IPlugin;
class Plugin;

class PluginPrivate : public QObject
{
    Q_OBJECT

public:

    PluginPrivate(Plugin *plugin, Application *application, const QString &filename);
    virtual ~PluginPrivate();

    QStringList arrayToStringList(const QJsonArray &array);

    Plugin *const q;

    Application *application;
    QPluginLoader loader;
    QJsonObject metadata;
    QStringList dependencies;
    IPlugin *iplugin;
    bool initialized;

    QList<Plugin*> children;
};

#endif // LIBNITROSHARE_PLUGIN_P_H
