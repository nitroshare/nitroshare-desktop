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

class NITROSHARE_EXPORT DevicePrivate;

/**
 * @brief Peer available for transfers
 *
 * A device is created in response to a DeviceEnumerator emitting the
 * deviceUpdated signal. Enumerators can add their own properties to devices,
 * usually for the purpose of later establishing a connection to them through
 * a transport.
 */
class NITROSHARE_EXPORT Device : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString uuid READ uuid)

public:

    /// Property key for the device UUID
    static const QString UuidKey;

    /**
     * @brief Retrieve the device's unique identifier
     */
    QString uuid() const;

private:

    explicit Device(const QString &uuid);

    DevicePrivate *const d;
    friend class DeviceModel;
    friend class DeviceModelPrivate;
};

#endif // LIBNITROSHARE_DEVICE_H
