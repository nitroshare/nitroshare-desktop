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
#include <QMap>
#include <QObject>
#include <QTest>

#include <qmdnsengine/dns.h>
#include <qmdnsengine/record.h>

#define PARSE_RECORD(r) \
    quint16 offset = 0; \
    QMdnsEngine::Record record; \
    bool result = QMdnsEngine::parseRecord( \
        QByteArray(r, sizeof(r)), \
        offset, \
        record \
    )

#define WRITE_RECORD() \
    QByteArray packet; \
    quint16 offset; \
    NameMap nameMap; \
    QMdnsEngine::writeRecord(packet, offset, record, nameMap)

typedef QMap<QByteArray, quint16> NameMap;

const char NameSimple[] = {
    '\x04', '_', 't', 'c', 'p',
    '\x05', 'l', 'o', 'c', 'a', 'l',
    '\0'
};

const char NamePointer[] = {
    '\x04', '_', 't', 'c', 'p',
    '\x05', 'l', 'o', 'c', 'a', 'l',
    '\0',
    '\x04', 't', 'e', 's', 't',
    '\xc0', '\0'
};

const char NameCorrupt[] = {
    '\x03', '1', '2'
};

const char RecordA[] = {
    '\x04', 't', 'e', 's', 't', '\0',
    '\x00', '\x01',
    '\x80', '\x01',
    '\x00', '\x00', '\x0e', '\x10',
    '\x00', '\x04',
    '\x7f', '\x00', '\x00', '\x01'
};

const char RecordAAAA[] = {
    '\x04', 't', 'e', 's', 't', '\0',
    '\x00', '\x1c',
    '\x00', '\x01',
    '\x00', '\x00', '\x0e', '\x10',
    '\x00', '\x10',
    '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00',
    '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x01'
};

const char RecordPTR[] = {
    '\x04', 't', 'e', 's', 't', '\0',
    '\x00', '\x0c',
    '\x00', '\x01',
    '\x00', '\x00', '\x0e', '\x10',
    '\x00', '\x07',
    '\x05', 't', 'e', 's', 't', '2', '\0',
};

const char RecordSRV[] = {
    '\x04', 't', 'e', 's', 't', '\0',
    '\x00', '\x21',
    '\x00', '\x01',
    '\x00', '\x00', '\x0e', '\x10',
    '\x00', '\x0d',
    '\x00', '\x01', '\x00', '\x02', '\x00', '\x03',
    '\x05', 't', 'e', 's', 't', '2', '\0',
};

const char RecordTXT[] = {
    '\x04', 't', 'e', 's', 't', '\0',
    '\x00', '\x10',
    '\x00', '\x01',
    '\x00', '\x00', '\x0e', '\x10',
    '\x00', '\x06',
    '\x03', 'a', '=', 'a',
    '\x01', 'b'
};

const QByteArray Name("test.");
const quint32 Ttl = 3600;
const QHostAddress Ipv4Address("127.0.0.1");
const QHostAddress Ipv6Address("::1");
const QByteArray Target("test2.");
const quint16 Priority = 1;
const quint16 Weight = 2;
const quint16 Port = 3;
const QMap<QByteArray, QByteArray> Attributes{
    {"a", "a"},
    {"b", QByteArray()}
};

class TestDns : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void testParseName_data();
    void testParseName();

    void testWriteName_data();
    void testWriteName();

    void testParseRecordA();
    void testParseRecordAAAA();
    void testParseRecordPTR();
    void testParseRecordSRV();
    void testParseRecordTXT();

    void testWriteRecordA();
    void testWriteRecordAAAA();
    void testWriteRecordPTR();
    void testWriteRecordSRV();
    void testWriteRecordTXT();
};

void TestDns::testParseName_data()
{
    QTest::addColumn<QByteArray>("packet");
    QTest::addColumn<quint16>("initialOffset");
    QTest::addColumn<quint16>("correctOffset");
    QTest::addColumn<QByteArray>("correctName");
    QTest::addColumn<bool>("correctResult");

    QTest::newRow("simple")
            << QByteArray(NameSimple, sizeof(NameSimple))
            << static_cast<quint16>(0)
            << static_cast<quint16>(12)
            << QByteArray("_tcp.local.")
            << true;

    QTest::newRow("pointer")
            << QByteArray(NamePointer, sizeof(NamePointer))
            << static_cast<quint16>(12)
            << static_cast<quint16>(19)
            << QByteArray("test._tcp.local.")
            << true;

    QTest::newRow("corrupt data")
            << QByteArray(NameCorrupt, sizeof(NameCorrupt))
            << static_cast<quint16>(0)
            << static_cast<quint16>(0)
            << QByteArray()
            << false;
}

void TestDns::testParseName()
{
    QFETCH(QByteArray, packet);
    QFETCH(quint16, initialOffset);
    QFETCH(quint16, correctOffset);
    QFETCH(QByteArray, correctName);
    QFETCH(bool, correctResult);

    quint16 offset = initialOffset;
    QByteArray name;
    bool result = QMdnsEngine::parseName(packet, offset, name);

    QCOMPARE(result, correctResult);
    if (result) {
        QCOMPARE(offset, correctOffset);
        QCOMPARE(name, correctName);
    }
}

void TestDns::testWriteName_data()
{
    QTest::addColumn<QByteArray>("initialPacket");
    QTest::addColumn<quint16>("initialOffset");
    QTest::addColumn<quint16>("correctOffset");
    QTest::addColumn<QByteArray>("name");
    QTest::addColumn<NameMap>("nameMap");
    QTest::addColumn<QByteArray>("correctPacket");

    QTest::newRow("simple")
            << QByteArray()
            << static_cast<quint16>(0)
            << static_cast<quint16>(12)
            << QByteArray("_tcp.local.")
            << NameMap()
            << QByteArray(NameSimple, sizeof(NameSimple));

    QTest::newRow("pointer")
            << QByteArray(NameSimple, sizeof(NameSimple))
            << static_cast<quint16>(sizeof(NameSimple))
            << static_cast<quint16>(19)
            << QByteArray("test._tcp.local.")
            << NameMap{{"_tcp.local", 0}}
            << QByteArray(NamePointer, sizeof(NamePointer));
}

void TestDns::testWriteName()
{
    QFETCH(QByteArray, initialPacket);
    QFETCH(quint16, initialOffset);
    QFETCH(quint16, correctOffset);
    QFETCH(QByteArray, name);
    QFETCH(NameMap, nameMap);
    QFETCH(QByteArray, correctPacket);

    QByteArray packet = initialPacket;
    quint16 offset = initialOffset;
    QMdnsEngine::writeName(packet, offset, name, nameMap);

    QCOMPARE(packet, correctPacket);
}

void TestDns::testParseRecordA()
{
    PARSE_RECORD(RecordA);

    QCOMPARE(result, true);
    QCOMPARE(record.name(), Name);
    QCOMPARE(record.type(), static_cast<quint16>(QMdnsEngine::A));
    QCOMPARE(record.flushCache(), true);
    QCOMPARE(record.ttl(), Ttl);
    QCOMPARE(record.address(), Ipv4Address);
}

void TestDns::testParseRecordAAAA()
{
    PARSE_RECORD(RecordAAAA);

    QCOMPARE(result, true);
    QCOMPARE(record.type(), static_cast<quint16>(QMdnsEngine::AAAA));
    QCOMPARE(record.address(), Ipv6Address);
}

void TestDns::testParseRecordPTR()
{
    PARSE_RECORD(RecordPTR);

    QCOMPARE(result, true);
    QCOMPARE(record.type(), static_cast<quint16>(QMdnsEngine::PTR));
    QCOMPARE(record.target(), QByteArray("test2."));
}

void TestDns::testParseRecordSRV()
{
    PARSE_RECORD(RecordSRV);

    QCOMPARE(result, true);
    QCOMPARE(record.type(), static_cast<quint16>(QMdnsEngine::SRV));
    QCOMPARE(record.priority(), Priority);
    QCOMPARE(record.weight(), Weight);
    QCOMPARE(record.port(), Port);
    QCOMPARE(record.target(), Target);
}

void TestDns::testParseRecordTXT()
{
    PARSE_RECORD(RecordTXT);

    QCOMPARE(result, true);
    QCOMPARE(record.type(), static_cast<quint16>(QMdnsEngine::TXT));
    QCOMPARE(record.attributes(), Attributes);
}

void TestDns::testWriteRecordA()
{
    QMdnsEngine::Record record;
    record.setName(Name);
    record.setType(QMdnsEngine::A);
    record.setFlushCache(true);
    record.setTtl(Ttl);
    record.setAddress(Ipv4Address);

    WRITE_RECORD();

    QCOMPARE(packet, QByteArray(RecordA, sizeof(RecordA)));
}

void TestDns::testWriteRecordAAAA()
{
    QMdnsEngine::Record record;
    record.setName(Name);
    record.setType(QMdnsEngine::AAAA);
    record.setTtl(Ttl);
    record.setAddress(Ipv6Address);

    WRITE_RECORD();

    QCOMPARE(packet, QByteArray(RecordAAAA, sizeof(RecordAAAA)));
}

void TestDns::testWriteRecordPTR()
{
    QMdnsEngine::Record record;
    record.setName(Name);
    record.setType(QMdnsEngine::PTR);
    record.setTtl(Ttl);
    record.setTarget(Target);

    WRITE_RECORD();

    QCOMPARE(packet, QByteArray(RecordPTR, sizeof(RecordPTR)));
}

void TestDns::testWriteRecordSRV()
{
    QMdnsEngine::Record record;
    record.setName(Name);
    record.setType(QMdnsEngine::SRV);
    record.setTtl(Ttl);
    record.setPriority(Priority);
    record.setWeight(Weight);
    record.setPort(Port);
    record.setTarget(Target);

    WRITE_RECORD();

    QCOMPARE(packet, QByteArray(RecordSRV, sizeof(RecordSRV)));
}

void TestDns::testWriteRecordTXT()
{
    QMdnsEngine::Record record;
    record.setName(Name);
    record.setType(QMdnsEngine::TXT);
    record.setTtl(Ttl);
    for (auto i = Attributes.constBegin(); i != Attributes.constEnd(); ++i) {
        record.addAttribute(i.key(), i.value());
    }

    WRITE_RECORD();

    QCOMPARE(packet, QByteArray(RecordTXT, sizeof(RecordTXT)));
}

QTEST_MAIN(TestDns)
#include "TestDns.moc"
