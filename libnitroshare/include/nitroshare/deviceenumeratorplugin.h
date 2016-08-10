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

#ifndef LIBNITROSHARE_DEVICEENUMERATORPLUGIN_H
#define LIBNITROSHARE_DEVICEENUMERATORPLUGIN_H

#include <QObject>

#include <nitroshare/device.h>

#include "config.h"

#define DeviceEnumeratorInterface_iid "net.nitroshare.NitroShare.DeviceEnumeratorInterface"

/**
 * @brief Interface for enumerating devices available for transfer
 *
 * This class must be used when writing a plugin that "discovers" or "finds"
 * devices for transfers.
 */
class NITROSHARE_EXPORT DeviceEnumeratorPlugin : public QObject
{
    Q_OBJECT

public:

    /**
     * @brief Begin enumerating devices
     */
    virtual void start() = 0;

Q_SIGNALS:

    /**
     * @brief Indicate a new device was found
     * @param device newly discovered device
     */
    void deviceFound(Device *device);
};

#endif // LIBNITROSHARE_DEVICEENUMERATORPLUGIN_H
