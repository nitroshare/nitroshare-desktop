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

#ifndef LIBNITROSHARE_IPLUGIN_H
#define LIBNITROSHARE_IPLUGIN_H

#include <QObject>

#include <nitroshare/config.h>

#define Plugin_iid "net.nitroshare.NitroShare.Plugin"

class Application;

/**
 * @brief Interface for plugins to inherit
 *
 * Each plugin must include a class that inherits from this one. The
 * initialize() method prepares the plugin for use, allowing it to register
 * actions, handlers, etc. The cleanup() method reverses all of the actions
 * performed in initialize().
 */
class NITROSHARE_EXPORT IPlugin : public QObject
{
    Q_OBJECT

public:

    /**
     * @brief Initialize the plugin
     * @param application pointer to Application
     *
     * Once initialization is complete, the finishedInitialization() signal
     * must be emitted.
     */
    virtual void initialize(Application *application) = 0;

    /**
     * @brief Cleanup the plugin
     * @param application pointer to Application
     *
     * Once cleanup is complete, the finishedCleanup() signal must be emitted.
     */
    virtual void cleanup(Application *application) = 0;

Q_SIGNALS:

    /**
     * @brief Indicate that initialization is complete
     */
    void finishedInitialization();

    /**
     * @brief Indicate that cleanup is complete
     */
    void finishedCleanup();
};

#endif // LIBNITROSHARE_IPLUGIN_H
