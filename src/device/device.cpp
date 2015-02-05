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

#include <QDateTime>

#include "../util/settings.h"
#include "device.h"
#include "device_p.h"

DevicePrivate::DevicePrivate(const QString &uuid)
    : uuid(uuid),
      name(uuid),
      operatingSystem(QObject::tr("unknown")),
      port(0),
      lastPing(0)
{
}

Device::Device(const QString &uuid)
    : d(new DevicePrivate(uuid))
{
}

Device::~Device()
{
    delete d;
}

QString Device::uuid() const
{
    return d->uuid;
}

QString Device::name() const
{
    return d->name;
}

void Device::setName(const QString &name)
{
    d->name = name;
    emit nameChanged(d->name);
}

QString Device::operatingSystem() const
{
    return d->operatingSystem;
}

void Device::setOperatingSystem(const QString &operatingSystem)
{
    d->operatingSystem = operatingSystem;
}

QHostAddress Device::address() const
{
    return d->address;
}

quint16 Device::port() const
{
    return d->port;
}

bool Device::expired() const
{
    return QDateTime::currentMSecsSinceEpoch() - d->lastPing >=
            Settings::get<qint64>(Settings::BroadcastTimeout);
}

void Device::update(const QHostAddress &address, quint16 port)
{
    d->address = address;
    d->port = port;
    d->lastPing = QDateTime::currentMSecsSinceEpoch();
}
