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

#include <QTest>

#include <qmdnsengine/dns.h>
#include <qmdnsengine/hostname.h>
#include <qmdnsengine/provider.h>
#include <qmdnsengine/record.h>
#include <qmdnsengine/service.h>

#include "common/testserver.h"

const QByteArray Name = "Test";
const QByteArray Type = "_test._tcp.local.";
const QByteArray Fqdn = Name + "." + Type;
const quint16 Port = 1234;
const QByteArray Key = "key";
const QByteArray Value = "value";

class TestProvider : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void testProvider();
};

void TestProvider::testProvider()
{
    TestServer server;
    QMdnsEngine::Hostname hostname(&server);
    QMdnsEngine::Provider provider(&server, &hostname);

    // Create a service description and provide it
    QMdnsEngine::Service service;
    service.setName(Name);
    service.setType(Type);
    service.setPort(Port);
    service.setAttributes({{Key, Value}});
    provider.update(service);

    // Wait for the three records to be announced (3 on each protocol)
    QMdnsEngine::Record record;
    QTRY_VERIFY(server.cache()->lookupRecord(Type, QMdnsEngine::PTR, record));
    QCOMPARE(record.target(), Fqdn);
    QTRY_VERIFY(server.cache()->lookupRecord(Fqdn, QMdnsEngine::SRV, record));
    QCOMPARE(record.target(), hostname.hostname());
    QCOMPARE(record.port(), Port);
    QTRY_VERIFY(server.cache()->lookupRecord(Fqdn, QMdnsEngine::TXT, record));
    QCOMPARE(record.attributes(), service.attributes());
}

QTEST_MAIN(TestProvider)
#include "TestProvider.moc"
