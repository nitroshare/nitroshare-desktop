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

#include <qmdnsengine/abstractserver.h>
#include <qmdnsengine/browser.h>
#include <qmdnsengine/cache.h>
#include <qmdnsengine/dns.h>
#include <qmdnsengine/mdns.h>
#include <qmdnsengine/message.h>
#include <qmdnsengine/query.h>
#include <qmdnsengine/record.h>

#include "browser_p.h"

using namespace QMdnsEngine;

BrowserPrivate::BrowserPrivate(Browser *browser, AbstractServer *server, const QByteArray &type, Cache *existingCache)
    : QObject(browser),
      q(browser),
      server(server),
      type(type),
      cache(existingCache ? existingCache : new Cache(this))
{
    connect(server, &AbstractServer::messageReceived, this, &BrowserPrivate::onMessageReceived);
    connect(cache, &Cache::shouldQuery, this, &BrowserPrivate::onShouldQuery);
    connect(cache, &Cache::recordExpired, this, &BrowserPrivate::onRecordExpired);
    connect(&queryTimer, &QTimer::timeout, this, &BrowserPrivate::onQueryTimeout);
    connect(&serviceTimer, &QTimer::timeout, this, &BrowserPrivate::onServiceTimeout);

    queryTimer.setSingleShot(true);
    serviceTimer.setSingleShot(true);

    // Immediately begin browsing for services
    onQueryTimeout();
}

// TODO: multiple SRV records not supported

bool BrowserPrivate::updateService(const QByteArray &fqName)
{
    // Split the FQDN into service name and type
    int index = fqName.indexOf('.');
    QByteArray serviceName = fqName.left(index);
    QByteArray serviceType = fqName.mid(index + 1);

    // Immediately return if a PTR record does not exist
    Record ptrRecord;
    if (!cache->lookupRecord(serviceType, PTR, ptrRecord)) {
        return false;
    }

    // If a SRV record is missing, query for it (by returning true)
    Record srvRecord;
    if (!cache->lookupRecord(fqName, SRV, srvRecord)) {
        return true;
    }

    Service service;
    service.setName(serviceName);
    service.setType(serviceType);
    service.setHostname(srvRecord.target());
    service.setPort(srvRecord.port());

    // If TXT records are available for the service, add their values
    QList<Record> txtRecords;
    if (cache->lookupRecords(fqName, TXT, txtRecords)) {
        QMap<QByteArray, QByteArray> attributes;
        foreach (Record record, txtRecords) {
            for (auto i = record.attributes().constBegin();
                    i != record.attributes().constEnd(); ++i) {
                attributes.insert(i.key(), i.value());
            }
        }
        service.setAttributes(attributes);
    }

    // If the service existed, this is an update; otherwise it is a new
    // addition; emit the appropriate signal
    if (!services.contains(fqName)) {
        emit q->serviceAdded(service);
    } else if(services.value(fqName) != service) {
        emit q->serviceUpdated(service);
    }

    services.insert(fqName, service);

    return false;
}

void BrowserPrivate::onMessageReceived(const Message &message)
{
    if (!message.isResponse()) {
        return;
    }

    // Use a set to track all services that are updated in the message - this
    // avoids extraneous signals being emitted if SRV and TXT are provided
    QSet<QByteArray> updateNames;
    foreach (Record record, message.records()) {
        bool any = type == MdnsBrowseType;
        if (record.type() == PTR && record.name() == MdnsBrowseType) {
            cache->addRecord(record);
            ptrTargets.insert(record.target());
            serviceTimer.stop();
            serviceTimer.start(100);
        } else if ((record.type() == PTR && (any || record.name() == type)) ||
                (record.type() == SRV && (any || record.name().endsWith("." + type))) ||
                (record.type() == TXT && (any || record.name().endsWith("." + type)))) {
            cache->addRecord(record);
            switch (record.type()) {
            case PTR:
                updateNames.insert(record.target());
                break;
            case SRV:
            case TXT:
                updateNames.insert(record.name());
                break;
            }
        }
    }

    // For each of the services marked to be updated, perform the update and
    // make a list of all missing SRV records
    QSet<QByteArray> queryNames;
    foreach (QByteArray name, updateNames) {
        if (updateService(name)) {
            queryNames.insert(name);
        }
    }

    // Build and send a query for all of the SRV records
    if (queryNames.count()) {
        Message queryMessage;
        foreach (QByteArray name, queryNames) {
            Query query;
            query.setName(name);
            query.setType(SRV);
            queryMessage.addQuery(query);
            query.setType(TXT);
            queryMessage.addQuery(query);
        }
        server->sendMessageToAll(queryMessage);
    }
}

void BrowserPrivate::onShouldQuery(const Record &record)
{
    // Assume that all messages in the cache are still in use (by the browser)
    // and attempt to renew them immediately

    Query query;
    query.setName(record.name());
    query.setType(record.type());
    Message message;
    message.addQuery(query);
    server->sendMessageToAll(message);
}

void BrowserPrivate::onRecordExpired(const Record &record)
{
    // If the PTR or SRV record has expired for a service, then it must be
    // removed - TXT records on the other hand, cause an update

    QByteArray serviceName;
    switch (record.type()) {
    case PTR:
        serviceName = record.target();
        break;
    case SRV:
        serviceName = record.name();
        break;
    case TXT:
        updateService(record.name());
        return;
    }
    Service service = services.value(serviceName);
    if (!service.name().isNull() && (record.type() == PTR || record.type() == SRV)) {
        emit q->serviceRemoved(service);
        services.remove(serviceName);
    }
}

void BrowserPrivate::onQueryTimeout()
{
    Query query;
    query.setName(type);
    query.setType(PTR);
    Message message;
    message.addQuery(query);

    // TODO: including too many records could cause problems

    // Include all currently valid PTR records
    QList<Record> records;
    if (cache->lookupRecords(QByteArray(), PTR, records)) {
        foreach (Record record, records) {
            message.addRecord(record);
        }
    }

    server->sendMessageToAll(message);
    queryTimer.start(60 * 1000);
}

void BrowserPrivate::onServiceTimeout()
{
    if (ptrTargets.count()) {
        Message message;
        foreach (QByteArray target, ptrTargets) {
            Query query;
            query.setName(target);
            query.setType(PTR);
            message.addQuery(query);
        }

        // TODO: cached PTR records

        server->sendMessageToAll(message);

        ptrTargets.clear();
    }
}

Browser::Browser(AbstractServer *server, const QByteArray &type, Cache *cache, QObject *parent)
    : QObject(parent),
      d(new BrowserPrivate(this, server, type, cache))
{
}
