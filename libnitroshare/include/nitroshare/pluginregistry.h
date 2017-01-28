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

#ifndef LIBNITROSHARE_PLUGINREGISTRY_H
#define LIBNITROSHARE_PLUGINREGISTRY_H

#include <QList>
#include <QObject>

#include <nitroshare/config.h>

class Application;
class Plugin;

class NITROSHARE_EXPORT PluginRegistryPrivate;

/**
 * @brief Registry for application plugins
 *
 * This class provides lifecycle management for individual plugins. This
 * includes dependency resolution, initialization, unloading, etc.
 */
class NITROSHARE_EXPORT PluginRegistry : public QObject
{
    Q_OBJECT

public:

    /**
     * @brief Create a new plugin registry
     * @param application pointer to Application
     * @param parent QObject
     */
    PluginRegistry(Application *application, QObject *parent = nullptr);

    /**
     * @brief Retrieve a list of all plugins
     * @return plugin name list
     */
    QList<Plugin*> plugins() const;

    /**
     * @brief Retrieve a plugin by name
     * @param name plugin name
     * @return pointer to Plugin or nullptr if none
     */
    Plugin *pluginByName(const QString &name) const;

    /**
     * @brief Load plugins from the specified directories
     * @param directories list of directories to load plugins from
     */
    void loadPluginsFromDirectories(const QStringList &directories);

    /**
     * @brief Unload a plugin and those that depend on it
     * @param name plugin name
     */
    void unloadPlugin(const QString &name);

    /**
     * @brief Unload all plugins
     */
    void unloadAll();

Q_SIGNALS:

    /**
     * @brief Indicate that the specified plugin was loaded
     * @param name plugin name
     */
    void pluginLoaded(const QString &name);

    /**
     * @brief Indicate that the specified plugin was unloaded
     * @param name plugin name
     */
    void pluginUnloaded(const QString &name);

private:

    PluginRegistryPrivate *const d;
};

#endif // LIBNITROSHARE_PLUGINREGISTRY_H
