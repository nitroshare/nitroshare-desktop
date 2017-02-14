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
#include <QStringList>
#include <QVariantMap>

#include <nitroshare/config.h>

class NITROSHARE_EXPORT SettingsPrivate;

/**
 * @brief Interface for registering, storing, and retrieving settings
 *
 * Plugins use settings to persist values across sessions. Settings also allow
 * users to modify plugin behavior during runtime. This could be done through
 * the UI, for example, or through the HTTP API.
 *
 * Settings must be registered before use (typically during plugin
 * initialization). The registration process allows plugins to include
 * metadata, which can be used by other plugins to facilitate editing the
 * settings. For example, a plugin could display a dialog box, allowing the
 * user to modify settings.
 *
 * This class is *not* thread-safe and should only be used from the main
 * thread.
 */
class NITROSHARE_EXPORT Settings : public QObject
{
    Q_OBJECT

public:

    /// Default key
    static const QString DefaultKey;

    /**
     * @brief Create a new settings object
     * @param settings QSettings object to use
     * @param parent QObject
     */
    Settings(QSettings *settings, QObject *parent = nullptr);

    /**
     * @brief Add a setting
     * @param key unique identifier for the setting
     * @param metadata metadata for the setting
     */
    void addSetting(const QString &key, const QVariantMap &metadata = QVariantMap());

    /**
     * @brief Remove a setting
     * @param key unique identifier for the setting
     */
    void removeSetting(const QString &key);

    /**
     * @brief Retrieve a map of keys and metadata
     * @return map of settings
     */
    QVariantMap settings() const;

    /**
     * @brief Retrieve the value for the specified setting
     * @param key unique identifier for the setting
     * @return value for key
     */
    QVariant value(const QString &key) const;

    /**
     * @brief Set the value for the specified key
     * @param key unique identifier for the setting
     * @param value new value for key
     *
     * The settingsChanged() signal is only emitted if the value has actually
     * changed.
     */
    void setValue(const QString &key, const QVariant &value);

    /**
     * @brief Begin an operation on a group of settings
     *
     * This method will cause all signals to be suppressed until end() is
     * invoked.
     */
    void begin();

    /**
     * @brief End an operation on a group of settings
     *
     * All settings that have changed since begin() will cause the
     * settingsChanged() signal to be emitted with the list of keys that
     * changed value.
     */
    void end();

Q_SIGNALS:

    /**
     * @brief Indicate that settings have been added
     * @param keys name of keys which have been added
     */
    void settingsAdded(const QStringList &keys);

    /**
     * @brief Indicate that settings have been removed
     * @param keys name of keys which have been removed
     */
    void settingsRemoved(const QStringList &keys);

    /**
     * @brief Indicate that settings have changed value
     * @param keys name of keys whose value has changed
     */
    void settingsChanged(const QStringList &keys);

private:

    SettingsPrivate *const d;
};

#endif // LIBNITROSHARE_SETTINGS_H
