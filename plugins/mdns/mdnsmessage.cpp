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

#include "mdnsmessage.h"

MdnsMessage::MdnsMessage()
    : mProtocol(Mdns::Protocol::IPv4),
      mPort(0),
      mTransactionId(0),
      mResponse(false)
{
}

QHostAddress MdnsMessage::address() const
{
    return mAddress;
}

void MdnsMessage::setAddress(const QHostAddress &address)
{
    mAddress = address;
}

Mdns::Protocol MdnsMessage::protocol() const
{
    return mProtocol;
}

void MdnsMessage::setProtocol(Mdns::Protocol protocol)
{
    mProtocol = protocol;
}

quint16 MdnsMessage::port() const
{
    return mPort;
}

void MdnsMessage::setPort(quint16 port)
{
    mPort = port;
}

quint16 MdnsMessage::transactionId() const
{
    return mTransactionId;
}

void MdnsMessage::setTransactionId(quint16 transactionId)
{
    mTransactionId = transactionId;
}

bool MdnsMessage::isResponse() const
{
    return mResponse;
}

void MdnsMessage::setResponse(bool response)
{
    mResponse = response;
}

QList<MdnsQuery> MdnsMessage::queries() const
{
    return mQueries;
}

void MdnsMessage::addQuery(const MdnsQuery &query)
{
    mQueries.append(query);
}

QList<MdnsRecord> MdnsMessage::records() const
{
    return mRecords;
}

void MdnsMessage::addRecord(const MdnsRecord &record)
{
    mRecords.append(record);
}

MdnsMessage MdnsMessage::reply() const
{
    MdnsMessage message;
    message.setTransactionId(transactionId());
    message.setResponse(true);
    if (port() == Mdns::Port) {
        if (protocol() == Mdns::Protocol::IPv4) {
            message.setAddress(Mdns::Ipv4Address);
        } else {
            message.setAddress(Mdns::Ipv6Address);
        }
    } else {
        message.setAddress(address());
    }
    message.setProtocol(protocol());
    message.setPort(port());
    return message;
}
