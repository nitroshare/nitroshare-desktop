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

#ifndef LIBNITROSHARE_DEVICEMODEL_P_H
#define LIBNITROSHARE_DEVICEMODEL_P_H

#include <QList>
#include <QObject>
#include <QVariantMap>

#include <nitroshare/deviceenumerator.h>
#include <nitroshare/devicemodel.h>

#include "device.h"

class DeviceModelPrivate : public QObject
{
    Q_OBJECT

public:

    explicit DeviceModelPrivate(DeviceModel *model);
    virtual ~DeviceModelPrivate();

    int findDevice(const QString &uuid);
    bool removeEnumerator(QObject *enumerator, int index);

    DeviceModel *const q;

    QList<Device*> devices;

public Q_SLOTS:

    void onDeviceUpdated(const QString &uuid, const QVariantMap &properties);
    void onDeviceRemoved(const QString &uuid);
};

#endif // LIBNITROSHARE_DEVICEMODEL_P_H
