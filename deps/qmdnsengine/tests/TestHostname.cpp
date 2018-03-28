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
#include <QNetworkInterface>
#include <QSignalSpy>
#include <QTest>

#include <qmdnsengine/dns.h>
#include <qmdnsengine/hostname.h>
#include <qmdnsengine/message.h>
#include <qmdnsengine/query.h>
#include <qmdnsengine/record.h>

#include "common/testserver.h"

const quint16 Port = 1234;

class TestHostname : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void testAcquire();
    void testAnswer();
};

void TestHostname::testAcquire()
{
    TestServer server;
    QMdnsEngine::Hostname hostname(&server);
    QSignalSpy hostnameChangedSpy(&hostname, SIGNAL(hostnameChanged(QByteArray)));

    // Wait for the first query
    QTRY_VERIFY(server.receivedMessages().count() > 0);
    QMdnsEngine::Message probe = server.receivedMessages().at(0);
    QVERIFY(probe.queries().count() > 0);

    // Immediately indicate that name is taken
    QMdnsEngine::Record record;
    record.setName(probe.queries().at(0).name());
    record.setType(QMdnsEngine::A);
    record.setAddress(QHostAddress("127.0.0.1"));
    QMdnsEngine::Message message;
    message.addRecord(record);
    server.deliverMessage(message);

    // Wait for another attempt and verify the name is different
    QTRY_VERIFY(hostname.isRegistered());
    QCOMPARE(hostname.hostname(), probe.queries().at(0).name());
    QCOMPARE(hostnameChangedSpy.count(), 1);
}

void TestHostname::testAnswer()
{
    if (!QNetworkInterface::allAddresses().count()) {
        QSKIP("no addresses available");
    }
    QHostAddress address = QNetworkInterface::allAddresses().at(0);

    TestServer server;
    QMdnsEngine::Hostname hostname(&server);

    // Wait for the hostname to be acquired
    QTRY_VERIFY(hostname.isRegistered());
    server.clearReceivedMessages();

    // Build a query for the host appearing to come from the first address
    QMdnsEngine::Query query;
    query.setName(hostname.hostname());
    query.setType(QMdnsEngine::A);
    QMdnsEngine::Message message;
    message.setAddress(address);
    message.setPort(Port);
    message.addQuery(query);
    server.deliverMessage(message);

    // Ensure a valid reply was received
    QTRY_VERIFY(server.receivedMessages().count() > 0);
    QMdnsEngine::Message reply = server.receivedMessages().at(0);
    QCOMPARE(reply.address(), address);
    QCOMPARE(reply.port(), Port);
    QVERIFY(reply.records().count() > 0);
}

QTEST_MAIN(TestHostname)
#include "TestHostname.moc"
