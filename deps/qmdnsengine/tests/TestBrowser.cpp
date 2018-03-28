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

#include <QSignalSpy>
#include <QTest>

#include <qmdnsengine/browser.h>
#include <qmdnsengine/dns.h>
#include <qmdnsengine/mdns.h>
#include <qmdnsengine/message.h>
#include <qmdnsengine/query.h>
#include <qmdnsengine/record.h>
#include <qmdnsengine/service.h>

#include "common/testserver.h"
#include "common/util.h"

Q_DECLARE_METATYPE(QMdnsEngine::Service)

const QByteArray Name = "Test";
const QByteArray Type = "_test._tcp.local.";
const QByteArray Fqdn = Name + "." + Type;
const QByteArray Target = "Test.local.";
const quint16 Port = 1234;
const QByteArray Key = "key";
const QByteArray Value = "value";

class TestBrowser : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void initTestCase();
    void testBrowser();
    void testBrowsePtr();
};

void TestBrowser::initTestCase()
{
    qRegisterMetaType<QMdnsEngine::Service>("Service");
}

void TestBrowser::testBrowser()
{
    TestServer server;
    QMdnsEngine::Browser browser(&server, Type);

    QSignalSpy serviceAddedSpy(&browser, SIGNAL(serviceAdded(Service)));
    QSignalSpy serviceUpdatedSpy(&browser, SIGNAL(serviceUpdated(Service)));
    QSignalSpy serviceRemovedSpy(&browser, SIGNAL(serviceRemoved(Service)));

    // Wait for the PTR query
    QTRY_VERIFY(queryReceived(&server, Type, QMdnsEngine::PTR));
    server.clearReceivedMessages();

    // Transmit the PTR record
    {
        QMdnsEngine::Record record;
        record.setName(Type);
        record.setType(QMdnsEngine::PTR);
        record.setTarget(Fqdn);
        QMdnsEngine::Message message;
        message.setResponse(true);
        message.addRecord(record);
        server.deliverMessage(message);
    }

    // Wait for a SRV query
    QTRY_VERIFY(queryReceived(&server, Fqdn, QMdnsEngine::SRV));
    server.clearReceivedMessages();

    // Nothing should have been added yet
    QCOMPARE(serviceAddedSpy.count(), 0);

    // Transmit the SRV record
    {
        QMdnsEngine::Record record;
        record.setName(Fqdn);
        record.setType(QMdnsEngine::SRV);
        record.setTarget(Target);
        record.setPort(Port);
        QMdnsEngine::Message message;
        message.setResponse(true);
        message.addRecord(record);
        server.deliverMessage(message);
    }

    // The serviceAdded signal should have been emitted
    QCOMPARE(serviceAddedSpy.count(), 1);

    // Transmit a TXT record
    {
        QMdnsEngine::Record record;
        record.setName(Fqdn);
        record.setType(QMdnsEngine::TXT);
        record.setAttributes({{Key, Value}});
        QMdnsEngine::Message message;
        message.setResponse(true);
        message.addRecord(record);
        server.deliverMessage(message);
    }

    // The serviceAdded signal should NOT have been emitted and the
    // serviceUpdated signal should have been
    QCOMPARE(serviceAddedSpy.count(), 1);
    QCOMPARE(serviceUpdatedSpy.count(), 1);

    // Remove the PTR record
    {
        QMdnsEngine::Record record;
        record.setName(Type);
        record.setType(QMdnsEngine::PTR);
        record.setTtl(0);
        record.setTarget(Fqdn);
        QMdnsEngine::Message message;
        message.setResponse(true);
        message.addRecord(record);
        server.deliverMessage(message);
    }

    // The serviceRemoved signal should have been emitted
    QCOMPARE(serviceRemovedSpy.count(), 1);
}

void TestBrowser::testBrowsePtr()
{
    TestServer server;
    QMdnsEngine::Browser browser(&server, QMdnsEngine::MdnsBrowseType);
    Q_UNUSED(browser);

    // Wait for a query for service types
    QTRY_VERIFY(queryReceived(&server, QMdnsEngine::MdnsBrowseType, QMdnsEngine::PTR));

    // Send a PTR and SRV record
    QMdnsEngine::Record record;
    record.setName(QMdnsEngine::MdnsBrowseType);
    record.setType(QMdnsEngine::PTR);
    record.setTarget(Type);
    QMdnsEngine::Message message;
    message.setResponse(true);
    message.addRecord(record);
    server.deliverMessage(message);

    // Wait for the query for records of the specified type
    QTRY_VERIFY(queryReceived(&server, Type, QMdnsEngine::PTR));
}

QTEST_MAIN(TestBrowser)
#include "TestBrowser.moc"
