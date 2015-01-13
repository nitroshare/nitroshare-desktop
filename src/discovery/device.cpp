/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Nathan Osman
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
 **/

#include <QJsonDocument>
#include <QJsonObject>
#include <QVariantMap>

#include "../util/misc.h"
#include "../util/settings.h"
#include "config.h"
#include "device.h"

QByteArray Device::current()
{
    // TODO: when switching to Qt 5.4, switch to using QJsonObject's
    // initializer instead of converting a QVariantMap

    QVariantMap device {
        { "uuid", Settings::get(Settings::Discovery::UUID) },
        { "name", Settings::get(Settings::Discovery::Name) },
        { "version", NITROSHARE_VERSION },
        { "operating_system", currentOperatingSystem() },
        { "port", Settings::get(Settings::Transfer::Port) }
    };

    return QJsonDocument(QJsonObject::fromVariantMap(device)).toJson(QJsonDocument::Compact);
}

bool Device::deserialize(const QByteArray &data, Device &device)
{
    QJsonObject object = QJsonDocument::fromJson(data).object();

    if(!object.contains("uuid") || !object.contains("version") || !object.contains("port")) {
        return false;
    }

    device.uuid    = object.value("uuid").toString();
    device.version = object.value("version").toString();
    device.port    = object.value("port").toInt();

    if(device.version != NITROSHARE_VERSION) {
        return false;
    }

    if(object.contains("name")) {
        device.name = object.value("name").toString();
    } else {
        device.name = device.uuid;
    }

    if(object.contains("operating_system")) {
        device.operatingSystem = object.value("operating_system").toString();
    } else {
        device.operatingSystem = "unknown";
    }

    return true;
}
