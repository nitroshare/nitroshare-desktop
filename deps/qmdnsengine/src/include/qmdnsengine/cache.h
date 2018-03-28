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

#ifndef QMDNSENGINE_CACHE_H
#define QMDNSENGINE_CACHE_H

#include <QList>
#include <QObject>

#include "qmdnsengine_export.h"

namespace QMdnsEngine
{

class Record;

class QMDNSENGINE_EXPORT CachePrivate;

/**
 * @brief %Cache for DNS records
 *
 * Records are added to the cache using the addRecord() method which are then
 * stored in the cache until they are considered to have expired, at which
 * point they are purged. The shouldQuery() signal is used to indicate when a
 * record is approaching expiry and the recordExpired() signal indicates when
 * a record has expired (at which point it is removed).
 *
 * The cache can be queried to retrieve one or more records matching a given
 * type. For example, to retrieve all TXT records that match a given name:
 *
 * @code
 * Cache cache;
 *
 * QList<QMdnsEngine::Record> records;
 * cache.lookupRecords("My Service._http._tcp.local.", QMdnsEngine::TXT, records);
 *
 * foreach (QMdnsEngine::Record record, records) {
 *     qDebug() << "Record:" << record.name();
 * }
 * @endcode
 *
 * Alternatively, lookupRecord() can be used to find a single record.
 */
class QMDNSENGINE_EXPORT Cache : public QObject
{
    Q_OBJECT

public:

    /**
     * @brief Create an empty cache.
     */
    explicit Cache(QObject *parent = 0);

    /**
     * @brief Add a record to the cache
     * @param record add this record to the cache
     *
     * The TTL for the record will be added to the current time to calculate
     * when the record expires. Existing records of the same name and type
     * will be replaced, resetting their expiration.
     */
    void addRecord(const Record &record);

    /**
     * @brief Retrieve a single record from the cache
     * @param name name of record to retrieve or null for any
     * @param type type of record to retrieve or ANY for all types
     * @param record storage for the record retrieved
     * @return true if a record was retrieved
     *
     * Some record types allow multiple records to be stored with identical
     * names and types. This method will only retrieve the first matching
     * record. Use lookupRecords() to obtain all of the records.
     */
    bool lookupRecord(const QByteArray &name, quint16 type, Record &record) const;

    /**
     * @brief Retrieve multiple records from the cache
     * @param name name of records to retrieve or null for any
     * @param type type of records to retrieve or ANY for all types
     * @param records storage for the records retrieved
     * @return true if records were retrieved
     */
    bool lookupRecords(const QByteArray &name, quint16 type, QList<Record> &records) const;

Q_SIGNALS:

    /**
     * @brief Indicate that a record will expire soon and a new query should be issued
     * @param record reference to the record that will soon expire
     *
     * This signal is emitted when a record reaches approximately 50%, 85%,
     * 90%, and 95% of its lifetime.
     */
    void shouldQuery(const Record &record);

    /**
     * @brief Indicate that the specified record expired
     * @param record reference to the record that has expired
     */
    void recordExpired(const Record &record);

private:

    CachePrivate *const d;
};

}

#endif // QMDNSENGINE_CACHE_H
