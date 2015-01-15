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

#ifndef NS_MANAGER_H
#define NS_MANAGER_H

#include <QJsonObject>
#include <QMap>
#include <QSharedPointer>
#include <QTimer>

#include "../util/settings.h"
#include "device.h"
#include "listener.h"

class Manager : public QObject
{
    Q_OBJECT

public:

    Manager();

    void start();

signals:

    void deviceAdded(const Device &device);
    void deviceRemoved(const Device &device);

private slots:

    void checkTimeouts();
    void processPing(const QJsonObject &object);

    void settingChanged(Settings::Key key);

private:

    void reload();

    QTimer timer;
    Listener listener;

    QMap<QString, QSharedPointer<Device>> devices;
};

#endif // NS_MANAGER_H
