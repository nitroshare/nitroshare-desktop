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

#include <QNetworkInterface>
#include <QTimer>

#include "../util/settings.h"
#include "interfacemonitor.h"

void InterfaceMonitor::start()
{
    refresh();
}

void InterfaceMonitor::refresh()
{
    QSet<QString> newNames;

    foreach(QNetworkInterface interface, QNetworkInterface::allInterfaces()) {
        if(interface.flags() && QNetworkInterface::CanMulticast)
            newNames.insert(interface.name());

        // TODO: verify that an IPv6 address is available since the
        // interface may only provide an IPv4 multicast address
    }

    foreach(QString name, newNames - oldNames) {
        emit interfaceAdded(name);
    }

    foreach(QString name, oldNames - newNames) {
        emit interfaceRemoved(name);
    }

    oldNames = newNames;

    QTimer::singleShot(Settings::get(Settings::Discovery::InterfaceMonitorInterval).toInt(),
                       this, SLOT(refresh()));
}
