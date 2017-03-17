/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Nathan Osman
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

#ifndef MDNSSERVER_H
#define MDNSSERVER_H

#include <QHostAddress>
#include <QList>
#include <QObject>
#include <QTimer>
#include <QUdpSocket>

#include "dnsmessage.h"

/**
 * @brief Bare-bones mDNS server
 */
class MdnsServer : public QObject
{
    Q_OBJECT

public:

    /**
     * @brief Create an mDNS server
     */
    MdnsServer();

    /**
     * @brief Start the server
     * @return true if the server was started
     */
    bool start();

    /**
     * @brief Stop the server
     */
    void stop();

    /**
     * @brief Send a DNS message
     */
    void sendMessage(const DnsMessage &message);

    /**
     * @brief Retrieve addresses that a host could use to connect
     */
    QList<QHostAddress> addressesForHost(const QHostAddress &hostAddress) const;

Q_SIGNALS:

    /**
     * @brief Indicate a new DNS message has been recieved
     */
    void messageReceived(const DnsMessage &message);

private Q_SLOTS:

    void onTimeout();
    void onReadyRead();

private:

    bool bindSocket(QUdpSocket &socket, const QHostAddress &address);

    QTimer mTimer;
    QUdpSocket mIpv4Socket;
    QUdpSocket mIpv6Socket;
};

#endif // MDNSSERVER_H
