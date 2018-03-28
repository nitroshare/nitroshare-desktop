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

#include <QObject>
#include <QSignalSpy>
#include <QTest>

#include <qmdnsengine/dns.h>
#include <qmdnsengine/cache.h>
#include <qmdnsengine/record.h>

Q_DECLARE_METATYPE(QMdnsEngine::Record)

const QByteArray Name = "Test";
const quint16 Type = QMdnsEngine::TXT;

class TestCache : public QObject
{
    Q_OBJECT

public:

    TestCache() : mCounter(0) {}

private Q_SLOTS:

    void initTestCase();
    void testExpiry();
    void testRemoval();
    void testCacheFlush();

private:

    QMdnsEngine::Record createRecord();

    int mCounter;
};

void TestCache::initTestCase()
{
    qRegisterMetaType<QMdnsEngine::Record>("Record");
}

void TestCache::testExpiry()
{
    QMdnsEngine::Cache cache;
    cache.addRecord(createRecord());

    QSignalSpy shouldQuerySpy(&cache, SIGNAL(shouldQuery(Record)));
    QSignalSpy recordExpiredSpy(&cache, SIGNAL(recordExpired(Record)));

    // The record should be in the cache
    QMdnsEngine::Record record;
    QVERIFY(cache.lookupRecord(Name, Type, record));

    // This unfortunately delays the test but 1s is the smallest TTL
    QTest::qWait(1100);

    // After entering the event loop, the record should have been purged and
    // the recordExpired() signal emitted
    QVERIFY(!cache.lookupRecord(Name, Type, record));
    QVERIFY(shouldQuerySpy.count() > 0);
    QCOMPARE(recordExpiredSpy.count(), 1);
}

void TestCache::testRemoval()
{
    QMdnsEngine::Cache cache;
    QMdnsEngine::Record record = createRecord();
    cache.addRecord(record);

    QSignalSpy recordExpiredSpy(&cache, SIGNAL(recordExpired(Record)));

    // Purge the record from the cache by setting its TTL to 0
    record.setTtl(0);
    cache.addRecord(record);

    // Verify that the record is gone
    QVERIFY(!cache.lookupRecord(Name, Type, record));
    QCOMPARE(recordExpiredSpy.count(), 1);
}

void TestCache::testCacheFlush()
{
    QMdnsEngine::Cache cache;
    for (int i = 0; i < 2; ++i) {
        cache.addRecord(createRecord());
    }

    QList<QMdnsEngine::Record> records;
    QVERIFY(cache.lookupRecords(Name, Type, records));
    QCOMPARE(records.length(), 2);

    // Insert a new record with the cache clear bit set
    QMdnsEngine::Record record = createRecord();
    record.setFlushCache(true);
    cache.addRecord(record);

    // Confirm that only a single record exists
    records.clear();
    QVERIFY(cache.lookupRecords(Name, Type, records));
    QCOMPARE(records.length(), 1);
}

QMdnsEngine::Record TestCache::createRecord()
{
    QMdnsEngine::Record record;
    record.setName(Name);
    record.setType(Type);
    record.setTtl(1);
    record.addAttribute("key", QByteArray::number(mCounter++));
    return record;
}

QTEST_MAIN(TestCache)
#include "TestCache.moc"
