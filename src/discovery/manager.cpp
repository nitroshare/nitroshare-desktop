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

#include "config.h"
#include "manager.h"

Manager::Manager()
{
    connect(&timer, &QTimer::timeout, this, &Manager::checkTimeouts);
    connect(&listener, &Listener::pingReceived, this, &Manager::processPing);

    reload();
}

void Manager::start()
{
    listener.start();
    timer.start();
}

void Manager::checkTimeouts()
{
    QMutableMapIterator<QString, QSharedPointer<Device>> i(devices);

    while(i.hasNext()) {
        if(i.next().value()->timeoutReached()) {
            emit deviceRemoved(*(i.value()));
            i.remove();
        }
    }
}

void Manager::processPing(const QJsonObject &object)
{
    if(object.contains("uuid") && object.contains("version")) {
        QString uuid(object.value("uuid").toString());
        QString version(object.value("version").toString());

        if(uuid != Settings::get<QString>(Settings::DeviceName) &&
                version == NITROSHARE_VERSION) {

            if(!devices.contains(uuid)) {
                QSharedPointer<Device> device(new Device(uuid));
                devices.insert(uuid, device);
                emit deviceAdded(*device);
            }
            devices.value(uuid)->update(object);
        }
    }
}

void Manager::settingChanged(Settings::Key key)
{
    if(key == Settings::BroadcastTimeout) {
        timer.stop();
        reload();
        timer.start();
    }
}

void Manager::reload()
{
    timer.setInterval(Settings::get<int>(Settings::BroadcastTimeout));
}
