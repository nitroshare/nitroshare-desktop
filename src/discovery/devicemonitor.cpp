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

#include <QMutableMapIterator>

#include "devicemonitor.h"

DeviceMonitor::DeviceMonitor()
{
    connect(&listener, &SocketListener::pingReceived, this, &DeviceMonitor::processPing);
    connect(&timer, &QTimer::timeout, this, &DeviceMonitor::checkTimeouts);

    reload();
}

void DeviceMonitor::start()
{
    listener.start();
    timer.start();
}

void DeviceMonitor::processPing(const Device &device)
{
    if(device.uuid != Settings::get<QString>(Settings::UUID)) {
        if(!devices.contains(device.uuid)) {
            emit deviceAdded(device);
        }
        devices.insert(device.uuid, device);
    }
}

void DeviceMonitor::checkTimeouts()
{
    QMutableMapIterator<QString, Device> i(devices);

    while(i.hasNext()) {
        if(i.next().value().hasTimedOut()) {
            emit deviceRemoved(i.value());
            i.remove();
        }
    }
}

void DeviceMonitor::settingChanged(Settings::Key key)
{
    if(key == Settings::DeviceTimeout) {
        timer.stop();
        reload();
        timer.start();
    }
}

void DeviceMonitor::reload()
{
    timer.setInterval(Settings::get<int>(Settings::DeviceTimeout));
}
