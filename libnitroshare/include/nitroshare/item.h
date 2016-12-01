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

#include "config.h"

/**
 * @brief Individual item for transfer
 *
 * Each item in a bundle is an instance of a class that derives from this one.
 * Properties should be used for attributes that need to be included in the
 * metadata sent in a transfer.
 */
class NITROSHARE_EXPORT Item : public QObject
{
    Q_OBJECT
    Q_ENUMS(OpenMode)
    Q_PROPERTY(QString type READ type)
    Q_PROPERTY(QString name READ name)
    Q_PROPERTY(qint64 size READ size)

public:

    /**
     * @brief Mode used for opening the file
     */
    enum OpenMode {
        Read,
        Write
    };

    /**
     * @brief Retrieve the unique identifier for the item
     * @return identifier
     */
    virtual QString type() const = 0;

    /**
     * @brief Retrieve the name of the item
     * @return item name
     */
    virtual QString name() const = 0;

    /**
     * @brief Retrieve the size of the item
     * @return item size
     */
    virtual qint64 size() const = 0;

    /**
     * @brief Open the item for reading or writing
     * @param openMode mode used for opening the file
     * @return true if the item was opened
     */
    virtual bool open(OpenMode openMode) = 0;

    /**
     * @brief Read data from the item
     * @return array of bytes
     *
     * This method should avoid returning large amounts of data in order to
     * avoid excess memory usage. Instead, return successive portions of the
     * item with each call.
     *
     * Use the error() signal to indicate an error.
     */
    virtual QByteArray read() = 0;

    /**
     * @brief Write data to the item
     * @param data information to write
     *
     * Use the error() signal to indicate an error.
     */
    virtual void write(const QByteArray &data) = 0;

    /**
     * @brief Close the item
     *
     * Use the error() signal to indicate an error.
     */
    virtual void close() = 0;

Q_SIGNALS:

    /**
     * @brief Indicate an error has occurred
     * @param message description of the error
     */
    void error(const QString &message);
};

#endif // LIBNITROSHARE_ITEM_H
