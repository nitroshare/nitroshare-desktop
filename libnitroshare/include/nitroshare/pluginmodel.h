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
     * @brief Clean up the plugin model
     */
    virtual ~PluginModel();

    /**
     * @brief Add plugins to the blacklist
     * @param names plugin names
     */
    void addToBlacklist(const QStringList &names);

    /**
     * @brief Add the specified plugin to the model
     * @param pointer to Plugin
     * @return true if the plugin was successfully added
     *
     * Note that the model assumes ownership of the plugin and will delete it
     * during cleanup. This applies even if adding fails.
     */
    bool add(Plugin *plugin);

    /**
     * @brief Load plugins from the specified directories
     * @param directories list of directories to load plugins from
     *
     * This method will initialize the new plugins after loading them.
     */
    void loadPluginsFromDirectories(const QStringList &directories);

    /**
     * @brief Find a plugin by name
     * @param name plugin name
     * @return pointer to Plugin or nullptr
     */
    Plugin *find(const QString &name) const;

    /**
     * @brief Attempt to load the specified plugin
     * @param plugin pointer to Plugin
     * @return true if the plugin was loaded
     */
    bool load(Plugin *plugin);

    /**
     * @brief Attempt to unload the specified plugin
     * @param plugin pointer to Plugin
     * @return true if the plugin was unloaded
     */
    bool unload(Plugin *plugin);

    /**
     * @brief Unload all plugins
     */
    void unloadAll();

    // Reimplemented virtual methods
    virtual int rowCount(const QModelIndex &parent) const;
    virtual QVariant data(const QModelIndex &index, int role) const;

private:

    PluginModelPrivate *const d;
};

#endif // LIBNITROSHARE_PLUGINMODEL_H
