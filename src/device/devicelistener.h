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

#ifndef NS_DEVICELISTENER_H
#define NS_DEVICELISTENER_H

#include <QHostAddress>
#include <QTimer>
#include <QUdpSocket>

#include "../settings/settings.h"
#include "../util/platform.h"

/**
 * @brief Device broadcast emitter and receiver
 *
 * This class listens for incoming broadcast packets and sends them out over
 * all active network interfaces at a configurable interval. When receiving
 * packets, the address is captured for later use when sending transfers.
 */
class DeviceListener : public QObject
{
    Q_OBJECT

public:

    DeviceListener();

Q_SIGNALS:

    void pingReceived(const QString &uuid, const QString &name, Platform::OperatingSystem operatingSystem,
                      const QHostAddress &address, quint16 port, bool usesTls);

private Q_SLOTS:

    void processPings();
    void sendPings();

    void onSettingsChanged(const QList<Settings::Key> &keys = {});

private:

    QTimer mTimer;
    QUdpSocket mSocket;
};

#endif // NS_DEVICELISTENER_H
