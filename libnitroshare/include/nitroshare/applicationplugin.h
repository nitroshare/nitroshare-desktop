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

#ifndef LIBNITROSHARE_APPLICATIONPLUGIN_H
#define LIBNITROSHARE_APPLICATIONPLUGIN_H

#include <QObject>

#include "config.h"

#define ApplicationPlugin_iid "net.nitroshare.NitroShare.ApplicationPlugin"

class Application;

/**
 * @brief Base class for plugins that add additional functionality
 *
 * Plugins provide the ability to add features to the application without
 * requiring the application to be rebuilt. Each plugin must provide a class
 * that extends this one.
 *
 * Whenever a plugin is loaded, the init() method is invoked and whenever the
 * plugin is being unloaded, the cleanup() method is invoked. The plugin has
 * access to most of the application functionality through the Application
 * pointer passed to init().
 */
class NITROSHARE_EXPORT ApplicationPlugin : public QObject
{
    Q_OBJECT

public:

    /**
     * @brief Initialize the plugin
     * @param application pointer to global application object
     */
    virtual void init(Application *application) = 0;

    /**
     * @brief Prepare the plugin for removal
     * @param application pointer to global application object
     *
     * This is an asynchronous request - the plugin need not complete the
     * cleanup process before returning. However, it should emit the
     * finishedCleanup() signal when it completes so that the plugin
     * can be unloaded.
     */
    virtual void cleanup(Application *application) = 0;

Q_SIGNALS:

    /**
     * @brief Indicate that cleanup has finished
     *
     * All resources must be freed when this signal is emitted since it
     * indicates to the plugin system that it is safe to unload the plugin.
     */
    void finishedCleanup();
};

#endif // LIBNITROSHARE_APPLICATIONPLUGIN_H
