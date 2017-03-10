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

#include "dnsmessage.h"
#include "dnsutil.h"

DnsMessage::DnsMessage(bool response)
    : mOkay(false),
      mResponse(response)
{
}

DnsMessage::DnsMessage(const QByteArray &message)
{
    mOkay = parse(message);
}

bool DnsMessage::isOkay() const
{
    return mOkay;
}

bool DnsMessage::isResponse() const
{
    return mResponse;
}

QList<DnsQuery> DnsMessage::queries() const
{
    return mQueries;
}

QList<DnsRecord> DnsMessage::records() const
{
    return mRecords;
}

void DnsMessage::addQuery(const DnsQuery &query)
{
    mQueries.append(query);
}

void DnsMessage::addRecord(const DnsRecord &record)
{
    mRecords.append(record);
}

QByteArray DnsMessage::toMessage() const
{
    QByteArray message;
    DnsUtil::writeInteger<quint16>(message, 0);
    DnsUtil::writeInteger<quint16>(message, mResponse ? 0x8400 : 0);
    DnsUtil::writeInteger<quint16>(message, mQueries.count());
    DnsUtil::writeInteger<quint16>(message, mRecords.count());
    DnsUtil::writeInteger<quint16>(message, 0);
    DnsUtil::writeInteger<quint16>(message, 0);
    foreach (DnsQuery query, mQueries) {
        writeQuery(message, query);
    }
    foreach (DnsRecord record, mRecords) {
        writeRecord(message, record);
    }
    return message;
}

void DnsMessage::writeQuery(QByteArray &message, const DnsQuery &query) const
{
    DnsUtil::writeName(message, query.name());
    DnsUtil::writeInteger<quint16>(message, query.type());
    DnsUtil::writeInteger<quint16>(message, query.unicastResponse() ? 0x8001 : 1);
}

void DnsMessage::writeRecord(QByteArray &message, const DnsRecord &record) const
{
    DnsUtil::writeName(message, record.name());
    DnsUtil::writeInteger<quint16>(message, record.type());
    DnsUtil::writeInteger<quint16>(message, record.flushCache() ? 0x8001 : 1);
    DnsUtil::writeInteger<quint32>(message, record.ttl());
    DnsUtil::writeInteger<quint16>(message, record.data().length());
    message.append(record.data());
}

bool DnsMessage::parse(const QByteArray &message)
{
    quint16 offset = 0;
    quint16 transactionId, flags, nQueries, nAnswers, nAuthRecs, nAddRecs;
    if (!DnsUtil::parseInteger<quint16>(message, offset, transactionId) ||
            !DnsUtil::parseInteger<quint16>(message, offset, flags) ||
            !DnsUtil::parseInteger<quint16>(message, offset, nQueries) ||
            !DnsUtil::parseInteger<quint16>(message, offset, nAnswers) ||
            !DnsUtil::parseInteger<quint16>(message, offset, nAuthRecs) ||
            !DnsUtil::parseInteger<quint16>(message, offset, nAddRecs)) {
        return false;
    }
    mResponse = flags & 0x8400;  // assume authority
    quint16 nRecords = nAnswers + nAuthRecs + nAddRecs;
    for (int i = 0; i < nQueries; ++i) {
        if (!parseQuery(message, offset)) {
            return false;
        }
    }
    for (int i = 0; i < nRecords; ++i) {
        if (!parseRecord(message, offset)) {
            return false;
        }
    }
    return true;
}

bool DnsMessage::parseQuery(const QByteArray &message, quint16 &offset)
{
    QByteArray name;
    if (!DnsUtil::parseName(message, offset, name)) {
        return false;
    }
    quint16 type, class_;
    if (!DnsUtil::parseInteger<quint16>(message, offset, type) ||
            !DnsUtil::parseInteger<quint16>(message, offset, class_)) {
        return false;
    }
    mQueries.append(DnsQuery(name, type, class_ & 0x8000));
    return true;
}

bool DnsMessage::parseRecord(const QByteArray &message, quint16 &offset)
{
    QByteArray name;
    if (!DnsUtil::parseName(message, offset, name)) {
        return false;
    }
    quint16 type, class_, nBytes;
    quint32 ttl;
    if (!DnsUtil::parseInteger<quint16>(message, offset, type) ||
            !DnsUtil::parseInteger<quint16>(message, offset, class_) ||
            !DnsUtil::parseInteger<quint32>(message, offset, ttl) ||
            !DnsUtil::parseInteger<quint16>(message, offset, nBytes) ||
            offset + nBytes > message.length()) {
        return false;
    }
    mRecords.append(DnsRecord(name, type, class_ & 0x8000, ttl, message, offset, nBytes));
    offset += nBytes;
    return true;
}
