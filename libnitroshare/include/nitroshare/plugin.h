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

#ifndef LIBNITROSHARE_PLUGIN_H
#define LIBNITROSHARE_PLUGIN_H

#include <QList>
#include <QObject>

#include <nitroshare/config.h>

class Application;

class NITROSHARE_EXPORT PluginPrivate;

/**
 * @brief Plugin providing additional functionality
 *
 * Plugins enable additional functionality without the need to recompile the
 * library. They also allow the code to be organized in a more modular fashion.
 *
 * Although this class will return a list of dependencies, it does not take
 * them into account when loading or unloading the plugin.
 */
class NITROSHARE_EXPORT Plugin : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isLoaded READ isLoaded)
    Q_PROPERTY(bool isInitialized READ isInitialized)
    Q_PROPERTY(QString name READ name)
    Q_PROPERTY(QString title READ title)
    Q_PROPERTY(QString vendor READ vendor)
    Q_PROPERTY(QString version READ version)
    Q_PROPERTY(QString description READ description)
    Q_PROPERTY(QStringList dependencies READ dependencies)

public:

    /**
     * @brief Create a plugin from the specified loader
     * @param application pointer to Application
     * @param filename plugin filename
     * @param parent parent QObject
     */
    Plugin(Application *application, const QString &filename, QObject *parent = nullptr);

    /**
     * @brief Determine if the plugin was loaded
     */
    bool isLoaded() const;

    /**
     * @brief Determine if the plugin was initialized
     */
    bool isInitialized() const;

    /**
     * @brief Retrieve the plugin's unique name
     */
    QString name() const;

    /**
     * @brief Retrieve the plugin's title
     */
    QString title() const;

    /**
     * @brief Retrieve the plugin's vendor
     */
    QString vendor() const;

    /**
     * @brief Retrieve the plugin's version
     */
    QString version() const;

    /**
     * @brief Retrieve a description of the plugin
     */
    QString description() const;

    /**
     * @brief Retrieve the plugin's dependencies
     */
    QStringList dependencies() const;

    /**
     * @brief Attempt to load the plugin from disk
     * @return true if the plugin was loaded
     *
     * This method has no effect if the plugin was already loaded.
     */
    bool load();

    /**
     * @brief Unload the plugin
     *
     * This method will ensure that the plugin is cleaned up before attempting
     * to unload it. This method has no effect if the plugin is not loaded.
     */
    void unload();

    /**
     * @brief Initialize the plugin
     *
     * This method has no effect if the plugin was already initialized or was
     * not first loaded.
     */
    void initialize();

    /**
     * @brief Cleanup the plugin
     *
     * This method has no effect if the plugin was not initialized.
     */
    void cleanup();

    /**
     * @brief Add the specified plugin as a child
     * @param plugin child plugin
     */
    void addChild(Plugin *plugin);

    /**
     * @brief Remove the specified plugin as a child
     * @param plugin child plugin
     */
    void removeChild(Plugin *plugin);

    /**
     * @brief Retrieve the list of child plugins
     */
    QList<Plugin*> children() const;

private:

    PluginPrivate *const d;
};

#endif // LIBNITROSHARE_PLUGIN_H
