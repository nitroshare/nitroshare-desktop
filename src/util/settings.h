/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Nathan Osman
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
 **/

#ifndef NS_SETTINGS_H
#define NS_SETTINGS_H

#include <QObject>
#include <QVariant>

class SettingsPrivate;

/**
 * @brief Interface for storing and retrieving settings
 *
 * Access to settings that control behavior of the application is done through
 * a global instance of this class. The global instance may be obtained
 * through the instance() static method. From there, settings can be both
 * retrieved and set. Settings are applied immediately.
 */
class Settings : public QObject
{
    Q_OBJECT

public:

    /**
     * @brief Create a new settings instance
     * @param parent parent QObject
     *
     * Instances of this class should not be instantiated. Instead, obtain a
     * pointer to the global instance with the instance() static method.
     */
    explicit Settings(QObject *parent = nullptr);

    /**
     * @brief Destroy the settings instance
     */
    virtual ~Settings();

    /// Time (in MS) between broadcast packets
    static const int BroadcastInterval;

    /// Port for sending broadcast packets
    static const int BroadcastPort;

    /// Time (in MS) after receiving a device's last packet before considering the device offline
    static const int BroadcastTimeout;

    /// Descriptive name of the device
    static const int DeviceName;

    /// Unique GUID used to identify the device
    static const int DeviceUUID;

    /// Size (in bytes) of the buffer used for transferring file data
    static const int TransferBuffer;

    /// Directory for storing received files and directories
    static const int TransferDirectory;

    /// Port for receiving transfers
    static const int TransferPort;

    /// Interval between consecutive update checks
    static const int UpdateInterval;

    /**
     * @brief Retrieve a pointer to the global settings instance
     * @return pointer to the global instance
     */
    static Settings *instance();

    /**
     * @brief Retrieve the value of the specified key
     * @param key key to retrieve the value of
     * @return value for the specified key
     */
    static QVariant get(int key);

    /**
     * @brief Set the value of the specified key
     * @param key key to set the value of
     * @param value new value for the key
     */
    static void set(int key, const QVariant &value);

    /**
     * @brief Reset all settings to their default values
     */
    static void reset();

Q_SIGNALS:

    /**
     * @brief Indicate that a specific setting has changed value
     * @param key key whose value has changed
     */
    void settingChanged(int key);

private:

    SettingsPrivate *const d;
};

#endif // NS_SETTINGS_H
