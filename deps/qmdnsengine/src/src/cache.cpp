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

#include <qmdnsengine/cache.h>
#include <qmdnsengine/dns.h>

#include "cache_p.h"

using namespace QMdnsEngine;

CachePrivate::CachePrivate(Cache *cache)
    : QObject(cache),
      q(cache)
{
    connect(&timer, &QTimer::timeout, this, &CachePrivate::onTimeout);

    timer.setSingleShot(true);
}

void CachePrivate::onTimeout()
{
    // Loop through all of the records in the cache, emitting the appropriate
    // signal when a trigger has passed, determining when the next trigger
    // will occur, and removing records that have expired
    QDateTime now = QDateTime::currentDateTime();
    QDateTime newNextTrigger;

    for (auto i = entries.begin(); i != entries.end();) {
        auto &triggers = (*i).triggers;

        // Loop through the triggers and remove ones that have already
        // passed
        bool shouldQuery = false;
        for (auto j = triggers.begin(); j != triggers.end();) {
            if ((*j) <= now) {
                shouldQuery = true;
                j = triggers.erase(j);
            } else {
                break;
            }
        }

        // If triggers remain, determine the next earliest one; if none
        // remain, the record has expired and should be removed
        if (triggers.length()) {
            if (newNextTrigger.isNull() || triggers.at(0) < newNextTrigger) {
                newNextTrigger = triggers.at(0);
            }
            if (shouldQuery) {
                emit q->shouldQuery((*i).record);
            }
            ++i;
        } else {
            emit q->recordExpired((*i).record);
            i = entries.erase(i);
        }
    }

    // If newNextTrigger contains a value, it will be the time for the next
    // trigger and the timer should be started again
    nextTrigger = newNextTrigger;
    if (!nextTrigger.isNull()) {
        timer.start(now.msecsTo(nextTrigger));
    }
}

Cache::Cache(QObject *parent)
    : QObject(parent),
      d(new CachePrivate(this))
{
}

void Cache::addRecord(const Record &record)
{
    bool flushCache = record.flushCache();
    bool ttlZero = record.ttl() == 0;
    for (auto i = d->entries.begin(); i != d->entries.end();) {
        if ((flushCache &&
             (*i).record.name() == record.name() &&
             (*i).record.type() == record.type()) ||
                (*i).record == record) {

            // If the TTL is set to 0, indicate that the record was removed
            if (ttlZero) {
                emit recordExpired((*i).record);
            }

            i = d->entries.erase(i);

            // No need to continue further if the TTL was set to 0
            if (ttlZero) {
                return;
            }
        } else {
            ++i;
        }
    }

    // Use the current time to calculate the triggers and add a random offset
    QDateTime now = QDateTime::currentDateTime();
    qint64 random = qrand() % 20;

    QList<QDateTime> triggers{
        now.addMSecs(record.ttl() * 500 + random),  // 50%
        now.addMSecs(record.ttl() * 850 + random),  // 85%
        now.addMSecs(record.ttl() * 900 + random),  // 90%
        now.addMSecs(record.ttl() * 950 + random),  // 95%
        now.addSecs(record.ttl())
    };

    // Append the record and its triggers
    d->entries.append({record, triggers});

    // Check if half of this record's lifetime is earlier than the next
    // scheduled trigger; if so, restart the timer
    if (d->nextTrigger.isNull() || triggers.at(0) < d->nextTrigger) {
        d->nextTrigger = triggers.at(0);
        d->timer.stop();
        d->timer.start(now.msecsTo(d->nextTrigger));
    }
}

bool Cache::lookupRecord(const QByteArray &name, quint16 type, Record &record) const
{
    QList<Record> records;
    if (lookupRecords(name, type, records)) {
        record = records.at(0);
        return true;
    }
    return false;
}

bool Cache::lookupRecords(const QByteArray &name, quint16 type, QList<Record> &records) const
{
    bool recordsAdded = false;
    foreach (CachePrivate::Entry entry, d->entries) {
        if ((name.isNull() || entry.record.name() == name) &&
                (type == ANY || entry.record.type() == type)) {
            records.append(entry.record);
            recordsAdded = true;
        }
    }
    return recordsAdded;
}
