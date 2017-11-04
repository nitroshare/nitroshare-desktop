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

#ifndef LIBNITROSHARE_SETTING_H
#define LIBNITROSHARE_SETTING_H

#include <QObject>
#include <QVariant>
#include <QVariantMap>

#include <nitroshare/config.h>

class NITROSHARE_EXPORT SettingPrivate;

/**
 * @brief Individual setting
 */
class NITROSHARE_EXPORT Setting : public QObject
{
    Q_OBJECT
    Q_ENUMS(Type)
    Q_PROPERTY(Type type READ type)
    Q_PROPERTY(QString name READ name)
    Q_PROPERTY(QString title READ title)
    Q_PROPERTY(bool isHidden READ isHidden)
    Q_PROPERTY(QString category READ category)
    Q_PROPERTY(QVariant defaultValue READ defaultValue)

public:

    /**
     * @brief Type of value used for setting
     */
    enum Type {
        Invalid,
        String,
        Integer,
        Boolean,
        FilePath,
        DirectoryPath
    };

    /// Key for the setting type
    static const QString TypeKey;

    /// Key for the setting's machine-friendly name
    static const QString NameKey;

    /// Key for the setting's human-friendly title
    static const QString TitleKey;

    /// Key for the visibility of the setting
    static const QString IsHiddenKey;

    /// Key for the category of the setting
    static const QString CategoryKey;

    /// Key for the setting's default value
    static const QString DefaultValueKey;

    /**
     * @brief Create a new setting with the specified properties
     * @param properties map of properties
     * @param parent QObject
     */
    Setting(const QVariantMap &properties, QObject *parent = nullptr);

    /**
     * @brief Retrieve the type of value stored in the setting
     */
    Type type() const;

    /**
     * @brief Retrieve the unique name of the setting
     */
    QString name() const;

    /**
     * @brief Retrieve a descriptive title for the setting
     */
    QString title() const;

    /**
     * @brief Determine if the setting should be hidden
     */
    bool isHidden() const;

    /**
     * @brief Retrieve the setting's category
     */
    QString category() const;

    /**
     * @brief Retrieve the default value of the setting
     */
    QVariant defaultValue() const;

private:

    SettingPrivate *const d;
};

Q_DECLARE_METATYPE(Setting::Type)

#endif // LIBNITROSHARE_SETTING_H
