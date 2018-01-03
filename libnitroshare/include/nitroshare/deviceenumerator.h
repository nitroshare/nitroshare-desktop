/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Nathan Osman
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

#ifndef LIBNITROSHARE_DEVICEENUMERATOR_H
#define LIBNITROSHARE_DEVICEENUMERATOR_H

#include <QList>
#include <QObject>
#include <QVariantMap>

#include <nitroshare/config.h>

class Device;

/**
 * @brief Enumerate devices available for transfers
 */
class NITROSHARE_EXPORT DeviceEnumerator : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name)

public:

    /**
     * @brief Retrieve the unique identifier for the enumerator
     */
    virtual QString name() const = 0;

Q_SIGNALS:

    /**
     * @brief Indicate a device has been added
     * @param device pointer to Device
     */
    void deviceAdded(Device *device);

    /**
     * @brief Indicate a device has been removed
     * @param device pointer to Device
     */
    void deviceRemoved(Device *device);
};

#endif // LIBNITROSHARE_DEVICEENUMERATOR_H
