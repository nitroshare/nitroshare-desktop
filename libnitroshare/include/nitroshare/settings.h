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

#ifndef LIBNITROSHARE_SETTINGS_H
#define LIBNITROSHARE_SETTINGS_H

#include <QSettings>

#include <nitroshare/config.h>

class NITROSHARE_EXPORT SettingsPrivate;

/**
 * @brief Interface for storing and retrieving settings for the application
 *
 * This class is *not* thread-safe.
 */
class NITROSHARE_EXPORT Settings : public QObject
{
    Q_OBJECT

public:

    /**
     * @brief Create a new settings object
     * @param settings QSettings object to use
     * @param parent QObject
     */
    Settings(QSettings *settings, QObject *parent = nullptr);

    /**
     * @brief Retrieve the value for the specified key
     * @param key name of value to retrieve
     * @param defaultValue default value for the key
     * @return value for key
     *
     * If the key exists, its value is returned. If the key does not exist,
     * the defaultValue function is invoked and the return value is used to
     * set the new value. The settingChanged() signal is *not* emitted when
     * this happens.
     */
    QVariant get(const QString &key, QVariant (*defaultValue)());

    /**
     * @brief Set the value for the specified key
     * @param key name of the value to set
     * @param value new value for key
     */
    void set(const QString &key, const QVariant &value);

    /**
     * @brief Begin changing a group of values
     *
     * The settingsChanged() signal will be supressed until end() is invoked.
     * This allows multiple values to easily be changed.
     */
    void begin();

    /**
     * @brief End changing a group of values
     *
     * All settings that have changed since begin() will cause the
     * settingsChanged() signal to be emitted with the list of keys that
     * changed value.
     */
    void end();

Q_SIGNALS:

    /**
     * @brief Indicate that settings have changed value
     * @param keys names of the values that have changed
     */
    void settingsChanged(const QStringList &keys);

private:

    SettingsPrivate *const d;
};

#endif // LIBNITROSHARE_SETTINGS_H
