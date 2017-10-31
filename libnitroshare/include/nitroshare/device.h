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

#ifndef LIBNITROSHARE_DEVICE_H
#define LIBNITROSHARE_DEVICE_H

#include <QObject>

#include <nitroshare/config.h>

/**
 * @brief Peer available for transfers
 *
 * A device is created by an enumerator.
 */
class NITROSHARE_EXPORT Device : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString uuid READ uuid)
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)

public:

    /**
     * @brief Retrieve the device's unique identifier
     */
    virtual QString uuid() const = 0;

    /**
     * @brief Retrieve the human-friendly name for the device
     */
    virtual QString name() const = 0;

    /**
     * @brief Retrieve the name of the transport that should be used
     * @return transport name or null string for none
     */
    virtual QString transportName() const = 0;

Q_SIGNALS:

    /**
     * @brief Indicate that the device name has changed
     * @param name new name
     */
    void nameChanged(const QString &name);
};

#endif // LIBNITROSHARE_DEVICE_H
