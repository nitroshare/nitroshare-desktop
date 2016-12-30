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

#ifndef LIBNITROSHARE_APPLICATION_H
#define LIBNITROSHARE_APPLICATION_H

#include <QObject>

#include <nitroshare/config.h>

class ActionModel;
class DeviceModel;
class HandlerRegistry;
class Logger;
class PluginModel;
class Settings;
class TransferModel;

class NITROSHARE_EXPORT ApplicationPrivate;

/**
 * @brief Runtime application manager
 *
 * This class acts as a glorified container for most of the other classes in
 * the library. By centralizing access to these instances, it becomes easier
 * to write plugins.
 */
class NITROSHARE_EXPORT Application : public QObject
{
    Q_OBJECT

public:

    /**
     * @brief Create a new application object
     * @param parent QObject
     */
    explicit Application(QObject *parent = nullptr);

    /**
     * @brief Retrieve the device UUID
     * @return unique device ID
     */
    QString deviceUuid() const;

    /**
     * @brief Retrieve the device name
     * @return name assigned to device
     */
    QString deviceName() const;

    /**
     * @brief Retrieve the application version
     * @return application version
     */
    QString version() const;

    /**
     * @brief Retrieve the global ActionModel instance
     * @return pointer to ActionModel
     */
    ActionModel *actionModel() const;

    /**
     * @brief Retrieve the global DeviceModel instance
     * @return pointer to DeviceModel
     */
    DeviceModel *deviceModel() const;

    /**
     * @brief Retrieve the global handler instance
     * @return pointer to HandlerRegistry
     */
    HandlerRegistry *handlerRegistry() const;

    /**
     * @brief Retrieve the global Logger instance
     * @return pointer to Logger
     */
    Logger *logger() const;

    /**
     * @brief Retrieve the global PluginModel instance
     * @return pointer to PluginModel
     */
    PluginModel *pluginModel() const;

    /**
     * @brief Retrieve the global settings instance
     * @return pointer to Settings
     */
    Settings *settings() const;

    /**
     * @brief Retrieve the global TransferModel instance
     * @return pointer to TransferModel
     */
    TransferModel *transferModel() const;

public Q_SLOTS:

    /**
     * @brief Shutdown the application
     */
    void quit();

private:

    ApplicationPrivate *const d;
};

#endif // LIBNITROSHARE_APPLICATION_H
