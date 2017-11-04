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

#ifndef LIBNITROSHARE_CATEGORY_H
#define LIBNITROSHARE_CATEGORY_H

#include <QObject>

#include <nitroshare/config.h>

class NITROSHARE_EXPORT CategoryPrivate;

/**
 * @brief Settings category used for grouping
 */
class NITROSHARE_EXPORT Category : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name)
    Q_PROPERTY(QString title READ title)

public:

    /// Key for the setting name
    static const QString NameKey;

    /// Key for the setting title
    static const QString TitleKey;

    /**
     * @brief Create a new category with the specified properties
     * @param properties map of properties
     * @param parent QObject
     */
    Category(const QVariantMap &properties, QObject *parent = nullptr);

    /**
     * @brief Retrieve the unique name of the category
     */
    QString name() const;

    /**
     * @brief Retrieve the title for the category
     */
    QString title() const;

private:

    CategoryPrivate *const d;
};

#endif // LIBNITROSHARE_CATEGORY_H
