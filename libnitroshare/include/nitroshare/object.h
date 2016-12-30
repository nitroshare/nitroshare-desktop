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

#ifndef LIBNITROSHARE_OBJECT_H
#define LIBNITROSHARE_OBJECT_H

#include <QObject>

#include <nitroshare/config.h>

/**
 * @brief Object that provides notifications when properties change
 *
 * Unfortunately, QObject does not provide a signal for changes to dynamic
 * properties. This class overrides the setProperty() method and causes it to
 * emit a signal when invoked.
 */
class NITROSHARE_EXPORT Object : public QObject
{
    Q_OBJECT

public:

    /**
     * @brief Create a new object
     * @param parent QObject
     */
    explicit Object(QObject *parent = nullptr);

    /**
     * @brief Set the specified property to the specified value
     * @param name property name
     * @param value property value
     */
    void setProperty(const char *name, const QVariant &value);

Q_SIGNALS:

    /**
     * @brief Indicate that a property value has been changed
     * @param name property name
     */
    void propertyChanged(const char *name);
};

#endif // LIBNITROSHARE_OBJECT_H
