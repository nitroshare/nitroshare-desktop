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

#ifndef LIBNITROSHARE_DEVICEENUMERATOR_H
#define LIBNITROSHARE_DEVICEENUMERATOR_H

#include <QObject>
#include <QVariantMap>

#include <nitroshare/device.h>

#include "config.h"

/**
 * @brief Enumerate devices available for transfers
 *
 * Plugins that provide discovery for devices must derive from this class and
 * use its signals to indicate when devices are added, updated, and removed.
 *
 * When a device is found, the deviceUpdated() signal should be used.
 * Likewise, when a device is no longer accessible, the deviceRemoved() signal
 * should be used. An IPv4 broadcast enumerator might, for example, emit the
 * deviceUpdated() signal when a UDP packet is received and the
 * deviceRemoved() signal after a predefined timeout.
 */
class NITROSHARE_EXPORT DeviceEnumerator : public QObject
{
    Q_OBJECT

Q_SIGNALS:

    /**
     * @brief Indicate a device has been added or updated
     * @param uuid unique identifier for the device
     * @param addresses peer addresses for the device
     * @param properties device properties
     */
    void deviceUpdated(const QString &uuid, const QStringList &addresses, const QVariantMap &properties);

    /**
     * @brief Indicate a device has been removed
     * @param uuid unique identifier for the device
     */
    void deviceRemoved(const QString &uuid);
};

struct NITROSHARE_EXPORT DeviceEnumeratorFactoryInterface
{
    virtual DeviceEnumerator *create() = 0;
    virtual ~DeviceEnumeratorFactoryInterface() {}
};

#define DeviceEnumeratorFactoryInterface_iid "net.nitroshare.NitroShare.DeviceEnumeratorFactoryInterface/1.0"
Q_DECLARE_INTERFACE(DeviceEnumeratorFactoryInterface, DeviceEnumeratorFactoryInterface_iid)

#endif // LIBNITROSHARE_DEVICEENUMERATOR_H
