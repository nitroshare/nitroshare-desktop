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

#ifndef LIBNITROSHARE_APPLICATION_P_H
#define LIBNITROSHARE_APPLICATION_P_H

#include <QMap>
#include <QSettings>

#include <nitroshare/actionmodel.h>
#include <nitroshare/application.h>
#include <nitroshare/devicemodel.h>
#include <nitroshare/logger.h>
#include <nitroshare/pluginmodel.h>
#include <nitroshare/settings.h>
#include <nitroshare/transfermodel.h>

class Handler;

class ApplicationPrivate : public QObject
{
    Q_OBJECT

public:

    explicit ApplicationPrivate(Application *application);

    Application *const q;

    QSettings baseSettings;

    ActionModel actionModel;
    DeviceModel deviceModel;
    Logger logger;
    PluginModel pluginModel;
    Settings settings;
    TransferModel transferModel;

    QMap<QString, Handler*> handlers;
};

#endif // LIBNITROSHARE_APPLICATION_P_H
