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

#ifndef LIBNITROSHARE_PLUGINMODEL_H
#define LIBNITROSHARE_PLUGINMODEL_H

#include <QAbstractListModel>
#include <QStringList>

#include <nitroshare/config.h>

class Application;
class Plugin;

class NITROSHARE_EXPORT PluginModelPrivate;

/**
 * @brief Model for application plugins
 *
 * This class provides lifecycle management for plugins. This includes
 * dependency resolution, initializing, unloading, etc.
 */
class NITROSHARE_EXPORT PluginModel : public QAbstractListModel
{
    Q_OBJECT

public:

    /**
     * @brief Create a new plugin model
     * @param application pointer to Application
     * @param parent QObject
     */
    PluginModel(Application *application, QObject *parent = nullptr);

    /**
     * @brief Retrieve a plugin by name
     * @param name plugin name
     * @return pointer to Plugin or nullptr if no match
     */
    Plugin *findPlugin(const QString &name);

    /**
     * @brief Load plugins from the specified directories
     * @param directories list of directories to load plugins from
     */
    void loadPluginsFromDirectories(const QStringList &directories);

    /**
     * @brief Unload a plugin and all others that depend on it
     * @param name plugin name
     */
    void unloadPlugin(const QString &name);

    /**
     * @brief Unload all plugins
     */
    void unloadAll();

    // Reimplemented virtual methods
    virtual int rowCount(const QModelIndex &parent) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual QHash<int, QByteArray> roleNames() const;

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

    PluginModelPrivate *const d;
};

#endif // LIBNITROSHARE_PLUGINMODEL_H
