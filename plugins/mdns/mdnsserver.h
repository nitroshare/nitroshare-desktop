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
#include <QObject>
#include <QTimer>
#include <QUdpSocket>

#include "mdns.h"
#include "mdnsmessage.h"
#include "mdnsrecord.h"

/**
 * @brief Bare-bones mDNS server
 *
 * A timer is used to monitor available network interfaces and join the
 * appropriate multicast group on new ones that appear. This is a workaround
 * for the fact that Qt doesn't provide signals for this.
 *
 * Upon creation, the server checks to see if the current hostname is being
 * used on the network. If the hostname is found to be in use, the server will
 * amend the name and try again. Once a name is found that doesn't conflict,
 * the server begins listening for mDNS queries.
 *
 * At this point, only the ".local" domain is supported.
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
     * @brief Retrieve the current hostname
     *
     * An empty string is returned if no valid hostname is in use.
     */
    QString hostname() const;

    /**
     * @brief Send a DNS message
     */
    void sendMessage(const MdnsMessage &message);

    /**
     * @brief Generate an A or AAAA record
     * @param address generate record relative to this address
     * @param type generate a record of this type
     * @param record reference to record to generate
     * @return true if a valid record was generated
     */
    bool generateRecord(const QHostAddress &address, quint16 type, MdnsRecord &record);

Q_SIGNALS:

    /**
     * @brief Indicate a new DNS message has been recieved
     */
    void messageReceived(const MdnsMessage &message);

    /**
     * @brief Indicate that an error has occurred
     */
    void error(const QString &message);

private Q_SLOTS:

    void onSocketTimeout();
    void onHostnameTimeout();

    void onReadyRead();
    void onMessageReceived(const MdnsMessage &message);

private:

    void bindSocket(QUdpSocket &socket, const QHostAddress &address);
    void checkHostname(Mdns::Protocol protocol);

    // Periodically check for new interfaces
    QTimer mSocketTimer;

    // Timeout for asserting a hostname
    QTimer mHostnameTimer;

    // Sockets used for sending and receiving messages
    QUdpSocket mIpv4Socket;
    QUdpSocket mIpv6Socket;

    // Members used for hostname checks
    QString mHostname;
    int mHostnameSuffix;
    bool mHostnameConfirmed;
};

#endif // MDNSSERVER_H
