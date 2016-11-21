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

#ifndef LIBNITROSHARE_ITEM_H
#define LIBNITROSHARE_ITEM_H

#include <QObject>
#include <QVariantMap>

class QIODevice;

/**
 * @brief Individual item for transfer
 *
 * Each item in a bundle is an instance of a class that derives from this one.
 */
class Item : public QObject
{
    Q_OBJECT

public:

    /**
     * @brief Key for retrieving item type
     */
    static const QString TypeKey;

    /**
     * @brief Key for retrieving item name
     */
    static const QString NameKey;

    /**
     * @brief Key for retrieving item size (in bytes)
     */
    static const QString SizeKey;

    /**
     * @brief Retrieve custom properties for the item
     * @return property map
     *
     * The map should (at minimum) include values for the TypeKey, NameKey,
     * and SizeKey keys.
     */
    virtual QVariantMap properties() const = 0;

    /**
     * @brief Set custom properties for the item
     *
     * This will be invoked before createWriter() is called.
     */
    virtual void setProperties(const QVariantMap &properties) = 0;

    /**
     * @brief Create a reader for the item
     * @return QIODevice
     */
    virtual QIODevice *createReader() = 0;

    /**
     * @brief Create a writer for the item
     * @return QIODevice
     */
    virtual QIODevice *createWriter() = 0;
};

#endif // LIBNITROSHARE_ITEM_H
