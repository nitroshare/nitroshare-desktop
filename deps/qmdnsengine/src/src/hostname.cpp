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

#include <QHostAddress>
#include <QHostInfo>
#include <QNetworkAddressEntry>
#include <QNetworkInterface>

#include <qmdnsengine/abstractserver.h>
#include <qmdnsengine/dns.h>
#include <qmdnsengine/hostname.h>
#include <qmdnsengine/message.h>
#include <qmdnsengine/query.h>
#include <qmdnsengine/record.h>

#include "hostname_p.h"

using namespace QMdnsEngine;

HostnamePrivate::HostnamePrivate(Hostname *hostname, AbstractServer *server)
    : QObject(hostname),
      q(hostname),
      server(server)
{
    connect(server, &AbstractServer::messageReceived, this, &HostnamePrivate::onMessageReceived);
    connect(&registrationTimer, &QTimer::timeout, this, &HostnamePrivate::onRegistrationTimeout);
    connect(&rebroadcastTimer, &QTimer::timeout, this, &HostnamePrivate::onRebroadcastTimeout);

    registrationTimer.setSingleShot(true);
    rebroadcastTimer.setSingleShot(true);

    // Immediately assert the hostname
    onRebroadcastTimeout();
}

void HostnamePrivate::resetHostname()
{
    hostnamePrev = hostname;
    hostnameRegistered = false;
    hostnameSuffix = 1;
}

void HostnamePrivate::assertHostname()
{
    // Begin with the local hostname and replace any "." with "-" (I'm looking
    // at you, macOS)
    QByteArray localHostname = QHostInfo::localHostName().toUtf8();
    localHostname = localHostname.replace('.', '-');

    // If the suffix > 1, then append a "-2", "-3", etc. to the hostname to
    // aid in finding one that is unique and not in use
    hostname = (hostnameSuffix == 1 ? localHostname:
        localHostname + "-" + QByteArray::number(hostnameSuffix)) + ".local.";

    // Compose a query for A and AAAA records matching the hostname
    Query ipv4Query;
    ipv4Query.setName(hostname);
    ipv4Query.setType(A);
    Query ipv6Query;
    ipv6Query.setName(hostname);
    ipv6Query.setType(AAAA);
    Message message;
    message.addQuery(ipv4Query);
    message.addQuery(ipv6Query);

    server->sendMessageToAll(message);

    // If no reply is received after two seconds, the hostname is available
    registrationTimer.stop();
    registrationTimer.start(2 * 1000);
}

bool HostnamePrivate::generateRecord(const QHostAddress &srcAddress, quint16 type, Record &record)
{
    // Attempt to find the interface that corresponds with the provided
    // address and determine this device's address from the interface

    foreach (QNetworkInterface interface, QNetworkInterface::allInterfaces()) {
        foreach (QNetworkAddressEntry entry, interface.addressEntries()) {
            if (srcAddress.isInSubnet(entry.ip(), entry.prefixLength())) {
                foreach (entry, interface.addressEntries()) {
                    QHostAddress address = entry.ip();
                    if ((address.protocol() == QAbstractSocket::IPv4Protocol && type == A) ||
                            (address.protocol() == QAbstractSocket::IPv6Protocol && type == AAAA)) {
                        record.setName(hostname);
                        record.setType(type);
                        record.setAddress(address);
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

void HostnamePrivate::onMessageReceived(const Message &message)
{
    if (message.isResponse()) {
        if (hostnameRegistered) {
            return;
        }
        foreach (Record record, message.records()) {
            if ((record.type() == A || record.type() == AAAA) && record.name() == hostname) {
                ++hostnameSuffix;
                assertHostname();
            }
        }
    } else {
        if (!hostnameRegistered) {
            return;
        }
        Message reply;
        reply.reply(message);
        foreach (Query query, message.queries()) {
            if ((query.type() == A || query.type() == AAAA) && query.name() == hostname) {
                Record record;
                if (generateRecord(message.address(), query.type(), record)) {
                    reply.addRecord(record);
                }
            }
        }
        if (reply.records().count()) {
            server->sendMessage(reply);
        }
    }
}

void HostnamePrivate::onRegistrationTimeout()
{
    hostnameRegistered = true;
    if (hostname != hostnamePrev) {
        emit q->hostnameChanged(hostname);
    }

    // Re-assert the hostname in half an hour
    rebroadcastTimer.start(30 * 60 * 1000);
}

void HostnamePrivate::onRebroadcastTimeout()
{
    resetHostname();
    assertHostname();
}

Hostname::Hostname(AbstractServer *server, QObject *parent)
    : QObject(parent),
      d(new HostnamePrivate(this, server))
{
}

bool Hostname::isRegistered() const
{
    return d->hostnameRegistered;
}

QByteArray Hostname::hostname() const
{
    return d->hostname;
}
