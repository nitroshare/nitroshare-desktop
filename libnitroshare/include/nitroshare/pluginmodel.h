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

    enum Role {
        NameRole = Qt::UserRole,
        TitleRole,
        VendorRole,
        VersionRole,
        DescriptionRole,
        DependenciesRole,
        IsLoadedRole,
        IsInitializedRole
    };

    /**
     * @brief Create a new plugin model
     * @param application pointer to Application
     * @param parent QObject
     */
    PluginModel(Application *application, QObject *parent = nullptr);

    /**
     * @brief Add a plugin to the blacklist
     * @param name plugin name
     */
    void addToBlacklist(const QString &name);

    /**
     * @brief Load plugins from the specified directories
     * @param directories list of directories to load plugins from
     *
     * This method loads plugins only and does not initialize them.
     */
    void loadPluginsFromDirectories(const QStringList &directories);

    /**
     * @brief Load the specified plugin
     * @param name plugin name
     * @return true if the plugin was loaded
     */
    bool loadPlugin(const QString &name);

    /**
     * @brief Unload the specified plugin and others that depend on it
     * @param name plugin name
     * @return true if the plugin was unloaded
     *
     * The plugin and all of its dependencies will first be cleaned up.
     */
    bool unloadPlugin(const QString &name);

    /**
     * @brief Initialize the specified plugin and its dependencies
     * @param name plugin name
     * @return true if the plugin was initialized
     *
     * This method will fail if any dependencies aren't loaded.
     */
    bool initializePlugin(const QString &name);

    /**
     * @brief Cleanup the specified plugin and others that depend on it
     * @param name plugin name
     * @return true if the plugin was cleaned up
     */
    bool cleanupPlugin(const QString &name);

    // Reimplemented virtual methods
    virtual int rowCount(const QModelIndex &parent) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual QHash<int, QByteArray> roleNames() const;

private:

    PluginModelPrivate *const d;
};

#endif // LIBNITROSHARE_PLUGINMODEL_H
