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
#include <QSignalSpy>
#include <QTest>

#include <qmdnsengine/dns.h>
#include <qmdnsengine/message.h>
#include <qmdnsengine/record.h>
#include <qmdnsengine/resolver.h>

#include "common/testserver.h"
#include "common/util.h"

Q_DECLARE_METATYPE(QHostAddress)

const QByteArray Name = "test.localhost.";
const QHostAddress Address("127.0.0.1");

class TestResolver : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void initTestCase();
    void testResolver();
};

void TestResolver::initTestCase()
{
    qRegisterMetaType<QHostAddress>("QHostAddress");
}

void TestResolver::testResolver()
{
    TestServer server;
    QMdnsEngine::Resolver resolver(&server, Name);
    QSignalSpy resolvedSpy(&resolver, SIGNAL(resolved(QHostAddress)));

    // Ensure two queries were dispatched
    QTRY_VERIFY(queryReceived(&server, Name, QMdnsEngine::A));
    QVERIFY(queryReceived(&server, Name, QMdnsEngine::AAAA));

    // Send a record with an address
    QMdnsEngine::Record record;
    record.setName(Name);
    record.setType(QMdnsEngine::A);
    record.setAddress(Address);
    QMdnsEngine::Message message;
    message.setResponse(true);
    message.addRecord(record);
    message.addRecord(record);
    server.deliverMessage(message);

    // Ensure resolved() was emitted with the right address
    QTRY_COMPARE(resolvedSpy.count(), 1);
    QCOMPARE(resolvedSpy.at(0).at(0).value<QHostAddress>(), Address);
}

QTEST_MAIN(TestResolver)
#include "TestResolver.moc"
