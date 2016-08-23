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

#include <QHostInfo>
#include <QUuid>

#include <nitroshare/application.h>

#include "application_p.h"

const QString DeviceUuid = "DeviceUuid";
const QString DeviceName = "DeviceName";

QVariant DeviceUuidDefault() { return QUuid::createUuid().toString(); }
QVariant DeviceNameDefault() { return QHostInfo::localHostName(); }

ApplicationPrivate::ApplicationPrivate(Application *application)
    : QObject(application),
      q(application),
      pluginModel(application),
      settings(&baseSettings)
{
}

Application::Application(QObject *parent)
    : QObject(parent),
      d(new ApplicationPrivate(this))
{
}

QString Application::deviceUuid() const
{
    return d->settings.get(DeviceUuid, &DeviceUuidDefault).toString();
}

QString Application::deviceName() const
{
    return d->settings.get(DeviceName, &DeviceNameDefault).toString();
}

QString Application::version() const
{
    return PROJECT_VERSION;
}

DeviceModel *Application::deviceModel() const
{
    return &d->deviceModel;
}

Logger *Application::logger() const
{
    return &d->logger;
}

PluginModel *Application::pluginModel() const
{
    return &d->pluginModel;
}

Settings *Application::settings() const
{
    return &d->settings;
}
