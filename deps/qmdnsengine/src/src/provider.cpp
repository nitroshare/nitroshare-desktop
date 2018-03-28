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
#include <qmdnsengine/dns.h>
#include <qmdnsengine/hostname.h>
#include <qmdnsengine/mdns.h>
#include <qmdnsengine/message.h>
#include <qmdnsengine/prober.h>
#include <qmdnsengine/provider.h>
#include <qmdnsengine/query.h>

#include "provider_p.h"

using namespace QMdnsEngine;

ProviderPrivate::ProviderPrivate(QObject *parent, AbstractServer *server, Hostname *hostname)
    : QObject(parent),
      server(server),
      hostname(hostname),
      prober(nullptr),
      initialized(false),
      confirmed(false)
{
    connect(server, &AbstractServer::messageReceived, this, &ProviderPrivate::onMessageReceived);
    connect(hostname, &Hostname::hostnameChanged, this, &ProviderPrivate::onHostnameChanged);

    browsePtrProposed.setName(MdnsBrowseType);
    browsePtrProposed.setType(PTR);
    ptrProposed.setType(PTR);
    srvProposed.setType(SRV);
    txtProposed.setType(TXT);
}

ProviderPrivate::~ProviderPrivate()
{
    if (confirmed) {
        farewell();
    }
}

void ProviderPrivate::announce()
{
    // Broadcast a message with each of the records

    Message message;
    message.setResponse(true);
    message.addRecord(ptrRecord);
    message.addRecord(srvRecord);
    message.addRecord(txtRecord);
    server->sendMessageToAll(message);
}

void ProviderPrivate::confirm()
{
    // Confirm that the desired name is unique through probing

    if (prober) {
        delete prober;
    }
    prober = new Prober(server, srvProposed, this);
    connect(prober, &Prober::nameConfirmed, [this](const QByteArray &name) {

        // If existing records were confirmed, indicate that they are no
        // longer valid
        if (confirmed) {
            farewell();
        } else {
            confirmed = true;
        }

        // Update the proposed records
        ptrProposed.setTarget(name);
        srvProposed.setName(name);
        txtProposed.setName(name);

        // Publish the proposed records and announce them
        publish();

        delete prober;
        prober = nullptr;
    });
}

void ProviderPrivate::farewell()
{
    // Send a message indicating that the existing records are no longer valid
    // by setting their TTL to 0

    ptrRecord.setTtl(0);
    srvRecord.setTtl(0);
    txtRecord.setTtl(0);
    announce();
}

void ProviderPrivate::publish()
{
    // Copy the proposed records over and announce them

    browsePtrRecord = browsePtrProposed;
    ptrRecord = ptrProposed;
    srvRecord = srvProposed;
    txtRecord = txtProposed;
    announce();
}

void ProviderPrivate::onMessageReceived(const Message &message)
{
    if (!confirmed || message.isResponse()) {
        return;
    }

    bool sendBrowsePtr = false;
    bool sendPtr = false;
    bool sendSrv = false;
    bool sendTxt = false;

    // Determine which records to send based on the queries
    foreach (Query query, message.queries()) {
        if (query.type() == PTR && query.name() == MdnsBrowseType) {
            sendBrowsePtr = true;
        } else if (query.type() == PTR && query.name() == ptrRecord.name()) {
            sendPtr = true;
        } else if (query.type() == SRV && query.name() == srvRecord.name()) {
            sendSrv = true;
        } else if (query.type() == TXT && query.name() == txtRecord.name()) {
            sendTxt = true;
        }
    }

    // Remove records to send if they are already known
    foreach (Record record, message.records()) {
        if (record == ptrRecord) {
            sendPtr = false;
        } else if (record == srvRecord) {
            sendSrv = false;
        } else if (record == txtRecord) {
            sendTxt = false;
        }
    }

    // Include the SRV and TXT if the PTR is being sent
    if (sendPtr) {
        sendSrv = sendTxt = true;
    }

    // If any records should be sent, compose a message reply
    if (sendBrowsePtr || sendPtr || sendSrv || sendTxt) {
        Message reply;
        reply.reply(message);
        if (sendBrowsePtr) {
            reply.addRecord(browsePtrRecord);
        }
        if (sendPtr) {
            reply.addRecord(ptrRecord);
        }
        if (sendSrv) {
            reply.addRecord(srvRecord);
        }
        if (sendTxt) {
            reply.addRecord(txtRecord);
        }
        server->sendMessage(reply);
    }
}

void ProviderPrivate::onHostnameChanged(const QByteArray &hostname)
{
    // Update the proposed SRV record
    srvProposed.setTarget(hostname);

    // If initialized, confirm the record
    if (initialized) {
        confirm();
    }
}

Provider::Provider(AbstractServer *server, Hostname *hostname, QObject *parent)
    : QObject(parent),
      d(new ProviderPrivate(this, server, hostname))
{
}

void Provider::update(const Service &service)
{
    d->initialized = true;

    // Clean the service name
    QByteArray serviceName = service.name();
    serviceName = serviceName.replace('.', '-');

    // Update the proposed records
    QByteArray fqName = serviceName + "." + service.type();
    d->browsePtrProposed.setTarget(service.type());
    d->ptrProposed.setName(service.type());
    d->ptrProposed.setTarget(fqName);
    d->srvProposed.setName(fqName);
    d->srvProposed.setPort(service.port());
    d->srvProposed.setTarget(d->hostname->hostname());
    d->txtProposed.setName(fqName);
    d->txtProposed.setAttributes(service.attributes());

    // Assuming a valid hostname exists, check to see if the new service uses
    // a different name - if so, it must first be confirmed
    if (d->hostname->isRegistered()) {
        if (!d->confirmed || fqName != d->srvRecord.name()) {
            d->confirm();
        } else {
            d->publish();
        }
    }
}
