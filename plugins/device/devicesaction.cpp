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

#include <nitroshare/application.h>
#include <nitroshare/device.h>
#include <nitroshare/devicemodel.h>
#include <nitroshare/jsonutil.h>

#include "devicesaction.h"

DevicesAction::DevicesAction(Application *application)
    : mApplication(application)
{
}

QString DevicesAction::name() const
{
    return "devices";
}

QString DevicesAction::description() const
{
    return tr(
        "Retrieve a list of devices currently available for transfer. "
        "This action takes no parameters and returns an array of devices.\n\n"
        "Each device object consists of the properties present in the device."
    );
}

QVariant DevicesAction::invoke(const QVariantMap &)
{
    QVariantList devices;
    DeviceModel *model = mApplication->deviceModel();
    for (int i = 0; i < model->rowCount(); ++i) {
        Device *device = model->data(model->index(i, 0), Qt::UserRole).value<Device*>();
        devices.append(JsonUtil::objectToJson(device));
    }
    return devices;
}
