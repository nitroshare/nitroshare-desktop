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

#include <qmdnsengine/mdns.h>
#include <qmdnsengine/message.h>
#include <qmdnsengine/query.h>
#include <qmdnsengine/record.h>

#include "message_p.h"

using namespace QMdnsEngine;

MessagePrivate::MessagePrivate()
    : port(0),
      transactionId(0),
      isResponse(false)
{
}

Message::Message()
    : d(new MessagePrivate)
{
}

Message::Message(const Message &other)
    : d(new MessagePrivate)
{
    *this = other;
}

Message &Message::operator=(const Message &other)
{
    *d = *other.d;
    return *this;
}

Message::~Message()
{
    delete d;
}

QHostAddress Message::address() const
{
    return d->address;
}

void Message::setAddress(const QHostAddress &address)
{
    d->address = address;
}

quint16 Message::port() const
{
    return d->port;
}

void Message::setPort(quint16 port)
{
    d->port = port;
}

quint16 Message::transactionId() const
{
    return d->transactionId;
}

void Message::setTransactionId(quint16 transactionId)
{
    d->transactionId = transactionId;
}

bool Message::isResponse() const
{
    return d->isResponse;
}

void Message::setResponse(bool isResponse)
{
    d->isResponse = isResponse;
}

QList<Query> Message::queries() const
{
    return d->queries;
}

void Message::addQuery(const Query &query)
{
    d->queries.append(query);
}

QList<Record> Message::records() const
{
    return d->records;
}

void Message::addRecord(const Record &record)
{
    d->records.append(record);
}

void Message::reply(const Message &other)
{
    if (other.port() == MdnsPort) {
        if (other.address().protocol() == QAbstractSocket::IPv4Protocol) {
            setAddress(MdnsIpv4Address);
        } else {
            setAddress(MdnsIpv6Address);
        }
    } else {
        setAddress(other.address());
    }
    setPort(other.port());
    setTransactionId(other.transactionId());
    setResponse(true);
}
