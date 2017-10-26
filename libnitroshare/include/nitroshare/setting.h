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
    Q_PROPERTY(QVariant defaultValue READ defaultValue)

public:

    /**
     * @brief Type of value used for setting
     */
    enum Type {
        String,
        Integer,
        Boolean,
    };

    /**
     * @brief Create a new setting
     * @param type setting type
     * @param name setting name
     * @param defaultValue default value
     * @param parent QObject
     */
    Setting(Type type, const QString &name, const QVariant &defaultValue, QObject *parent = nullptr);

    /**
     * @brief Retrieve the type of value stored in the setting
     */
    Type type() const;

    /**
     * @brief Retrieve the unique name of the setting
     */
    QString name() const;

    /**
     * @brief Retrieve the default value of the setting
     */
    QVariant defaultValue() const;

private:

    SettingPrivate *const d;
};

#endif // LIBNITROSHARE_SETTING_H
