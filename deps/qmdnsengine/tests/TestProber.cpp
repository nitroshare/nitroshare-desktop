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

#include <qmdnsengine/dns.h>
#include <qmdnsengine/message.h>
#include <qmdnsengine/prober.h>
#include <qmdnsengine/record.h>

#include "common/testserver.h"

const QByteArray Name = "Test._http._tcp.local.";
const quint16 Type = QMdnsEngine::SRV;

class TestProber : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void testProbe();
};

void TestProber::testProbe()
{
    QMdnsEngine::Record record;
    record.setName(Name);
    record.setType(Type);

    TestServer server;
    QMdnsEngine::Prober prober(&server, record);
    QSignalSpy nameConfirmedSpy(&prober, SIGNAL(nameConfirmed(QByteArray)));

    // Return an existing record to signal a conflict
    QMdnsEngine::Message message;
    message.setResponse(true);
    message.addRecord(record);
    server.deliverMessage(message);

    // Wait for the replacement name to be confirmed
    QTRY_COMPARE(nameConfirmedSpy.count(), 1);
    QVERIFY(nameConfirmedSpy.at(0).at(0).toByteArray() != Name);
}

QTEST_MAIN(TestProber)
#include "TestProber.moc"
