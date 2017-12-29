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

#include <QObject>

#include <nitroshare/config.h>

class NITROSHARE_EXPORT PluginPrivate;

/**
 * @brief Plugin providing additional functionality
 *
 * Plugins enable additional functionality without the need to recompile the
 * library. They also allow the code to be organized in a more modular fashion.
 *
 * The plugin registry is used to load and unload plugins.
 */
class NITROSHARE_EXPORT Plugin : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name)
    Q_PROPERTY(QString title READ title)
    Q_PROPERTY(QString vendor READ vendor)
    Q_PROPERTY(QString version READ version)
    Q_PROPERTY(QString description READ description)
    Q_PROPERTY(bool isLoaded READ isLoaded)

public:

    /**
     * @brief Create a plugin from the specified loader
     * @param filename plugin filename
     * @param parent parent QObject
     */
    Plugin(const QString &filename, QObject *parent = nullptr);

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
     * @brief Determine if the plugin was loaded
     *
     * This method returns true if the plugin was loaded and initialized. It
     * will return false if the plugin was loaded but not initialied.
     */
    bool isLoaded() const;

private:

    PluginPrivate *const d;
    friend class PluginModel;
};

#endif // LIBNITROSHARE_PLUGIN_H
