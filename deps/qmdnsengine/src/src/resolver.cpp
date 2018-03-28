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

#include <QTimer>

#include <qmdnsengine/abstractserver.h>
#include <qmdnsengine/dns.h>
#include <qmdnsengine/cache.h>
#include <qmdnsengine/message.h>
#include <qmdnsengine/query.h>
#include <qmdnsengine/record.h>
#include <qmdnsengine/resolver.h>

#include "resolver_p.h"

using namespace QMdnsEngine;

ResolverPrivate::ResolverPrivate(Resolver *resolver, AbstractServer *server, const QByteArray &name, Cache *cache)
    : QObject(resolver),
      q(resolver),
      server(server),
      name(name),
      cache(cache ? cache : new Cache(this))
{
    connect(server, &AbstractServer::messageReceived, this, &ResolverPrivate::onMessageReceived);
    connect(&timer, &QTimer::timeout, this, &ResolverPrivate::onTimeout);

    // Query for new records
    query();

    // Pull the existing records from the cache
    timer.setSingleShot(true);
    timer.start(0);
}

QList<Record> ResolverPrivate::existing() const
{
    QList<Record> records;
    cache->lookupRecords(name, A, records);
    cache->lookupRecords(name, AAAA, records);
    return records;
}

void ResolverPrivate::query() const
{
    Message message;

    // Add a query for A and AAAA records
    Query query;
    query.setName(name);
    query.setType(A);
    message.addQuery(query);
    query.setType(AAAA);
    message.addQuery(query);

    // Add existing (known) records to the query
    foreach (Record record, existing()) {
        message.addRecord(record);
    }

    // Send the query
    server->sendMessageToAll(message);
}

void ResolverPrivate::onMessageReceived(const Message &message)
{
    if (!message.isResponse()) {
        return;
    }
    foreach (Record record, message.records()) {
        if (record.name() == name && (record.type() == A || record.type() == AAAA)) {
            cache->addRecord(record);
            if (!addresses.contains(record.address())) {
                emit q->resolved(record.address());
                addresses.insert(record.address());
            }
        }
    }
}

void ResolverPrivate::onTimeout()
{
    foreach (Record record, existing()) {
        emit q->resolved(record.address());
    }
}

Resolver::Resolver(AbstractServer *server, const QByteArray &name, Cache *cache, QObject *parent)
    : QObject(parent),
      d(new ResolverPrivate(this, server, name, cache))
{
}
