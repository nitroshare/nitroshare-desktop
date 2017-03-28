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

#include "mdnsbroadcaster.h"

#include "mdnsmessage.h"
#include "mdnsserver.h"
#include "mdnsservice.h"

const QByteArray BrowsePTR = "_services._dns-sd._udp.local.";

MdnsBroadcaster::MdnsBroadcaster(MdnsServer *server, MdnsService *service)
    : mMdnsServer(server),
      mMdnsService(service)
{
    connect(server, &MdnsServer::messageReceived, this, &MdnsBroadcaster::onMessageReceived);
}

void MdnsBroadcaster::onMessageReceived(const MdnsMessage &message)
{
    if (message.isResponse()) {
        return;
    }
    const QByteArray fqdn = mMdnsService->name() + "." + mMdnsService->type();
    bool sendAddresses = false;
    bool sendPtr = false;
    bool sendPtrService = false;
    bool sendSrv = false;
    bool sendTxt = false;
    foreach (MdnsQuery query, message.queries()) {
        switch (query.type()) {
        case Mdns::PTR:
            if (query.name() == mMdnsService->type()) {
                sendAddresses = true;
                sendPtr = true;
                sendSrv = true;
                sendTxt = true;
            } else if (query.name() == BrowsePTR) {
                sendPtrService = true;
            }
            break;
        case Mdns::SRV:
            if (query.name() == fqdn) {
                sendAddresses = true;
                sendSrv = true;
            }
            break;
        case Mdns::TXT:
            if (query.name() == fqdn) {
                sendTxt = true;
            }
            break;
        default:
            break;
        }
    }
    if (!sendAddresses && !sendPtr && !sendPtrService && !sendSrv && !sendTxt) {
        return;
    }
    MdnsMessage reply = message.reply();
    if (sendAddresses) {
        MdnsRecord record;
        if (message.protocol() == Mdns::IPv4) {
            if (!mMdnsServer->generateRecord(message.address(), Mdns::A, record)) {
                return;
            }
        }
        if (message.protocol() == Mdns::IPv6) {
            if (!mMdnsServer->generateRecord(message.address(), Mdns::AAAA, record)) {
                return;
            }
        }
        reply.addRecord(record);
    }
    if (sendPtr) {
        reply.addRecord(generatePtr());
    }
    if (sendPtrService) {
        reply.addRecord(generatePtrService());
    }
    if (sendSrv) {
        reply.addRecord(generateSrv());
    }
    if (sendTxt) {
        reply.addRecord(generateTxt());
    }
    mMdnsServer->sendMessage(reply);
}

MdnsRecord MdnsBroadcaster::generatePtrService()
{
    MdnsRecord record;
    record.setName(BrowsePTR);
    record.setType(Mdns::PTR);
    record.setTtl(3600);
    record.setTarget(mMdnsService->type());
    return record;
}

MdnsRecord MdnsBroadcaster::generatePtr()
{
    MdnsRecord record;
    record.setName(mMdnsService->type());
    record.setType(Mdns::PTR);
    record.setTtl(3600);
    record.setTarget(mMdnsService->name() + "." + mMdnsService->type());
    return record;
}

MdnsRecord MdnsBroadcaster::generateSrv()
{
    MdnsRecord record;
    record.setName(mMdnsService->name() + "." + mMdnsService->type());
    record.setType(Mdns::SRV);
    record.setPort(mMdnsService->port());
    record.setTtl(3600);
    record.setTarget(mMdnsServer->hostname().toUtf8());
    return record;
}

MdnsRecord MdnsBroadcaster::generateTxt()
{
    MdnsRecord record;
    record.setName(mMdnsService->name() + "." + mMdnsService->type());
    record.setType(Mdns::TXT);
    record.setTtl(3600);
    record.setTxt(mMdnsService->attributes());
    return record;
}
