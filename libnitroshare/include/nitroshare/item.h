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

class QIODevice;

/**
 * @brief Individual item for transfer
 */
class Item : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString type READ type)
    Q_PROPERTY(QString name READ name)
    Q_PROPERTY(qint64 size READ size)

public:

    /**
     * @brief Item type
     * @return string
     */
    virtual QString type() const = 0;

    /**
     * @brief Item name
     * @return string
     */
    virtual QString name() const = 0;

    /**
     * @brief Item size
     * @return size in bytes
     */
    virtual qint64 size() const = 0;

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
