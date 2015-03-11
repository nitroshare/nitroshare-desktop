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

#include <QList>
#include <QObject>

class SettingsPrivate;

/**
 * @brief Interface for storing and retrieving settings
 *
 * Access to settings that control behavior of the application is done through
 * a global instance of this class. The global instance may be obtained
 * through the instance() static method.
 *
 * Settings can be retrieved by providing the appropriate key to the get()
 * method. To change the value of a single setting, use the set() method. The
 * process for changing the value of multiple settings is as follows:
 *
 * - call the beginSet() method
 * - call the set() method for each individual setting
 * - call the endSet() method
 *
 * To receive notification when a value changes, obtain the global instance
 * and connect to the settingsChanged() signal.
 *
 * Note: this class is not thread safe.
 */
class Settings : public QObject
{
    Q_OBJECT

public:

    /**
     * @brief Helpful unit constants
     */
    enum Constant {
        Second = 1000,
        Minute = 60 * Second,
        Hour = 60 * Minute,
        KiB = 1024
    };

    /**
     * @brief Keys for individual settings
     */
    enum class Key : int {
        /// Port for sending broadcast packets
        BroadcastPort = 1,
        /// Time (in MS) between broadcast packets
        BroadcastInterval = 2,
        /// Time (in MS) after receiving a device's last packet before considering the device offline
        BroadcastTimeout = 3,
        /// Unique GUID used to identify the device
        DeviceUUID = 11,
        /// Descriptive name of the device
        DeviceName = 12,
        /// Port for receiving transfers
        TransferPort = 21,
        /// Size (in bytes) of the buffer used for transferring file data
        TransferBuffer = 22,
        /// Directory for storing received files and directories
        TransferDirectory = 23,
        /// Whether to check for updates or not
        UpdateCheck = 31,
        /// Interval between consecutive update checks
        UpdateInterval = 32
    };

    /**
     * @brief Retrieve the value of the specified key
     * @param key key to retrieve the value of
     * @return value for the specified key
     */
    QVariant get(Key key);

    /**
     * @brief Begin changing a group of settings
     */
    void beginSet();

    /**
     * @brief Set the value of the specified key
     * @param key key to set the value of
     * @param value new value for the key
     */
    void set(Key key, const QVariant &value);

    /**
     * @brief Finish changing a group of settings
     */
    void endSet();

    /**
     * @brief Reset all settings to their default values
     */
    void reset();

    /**
     * @brief Retrieve a pointer to the global settings instance
     * @return pointer to the global instance
     */
    static Settings *instance();

Q_SIGNALS:

    /**
     * @brief Indicate that one or more settings have changed
     * @param keys list of keys with new values
     */
    void settingsChanged(const QList<Key> &keys);

private:

    // Prevent the creation or deletion of class instances
    Settings();
    virtual ~Settings();

    SettingsPrivate *const d;
};

#endif // NS_SETTINGS_H
