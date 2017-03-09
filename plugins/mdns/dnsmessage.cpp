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
    writeInteger<quint16>(message, 0);
    writeInteger<quint16>(message, mResponse ? 0x8400 : 0);
    writeInteger<quint16>(message, mQueries.count());
    writeInteger<quint16>(message, mRecords.count());
    writeInteger<quint16>(message, 0);
    writeInteger<quint16>(message, 0);
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
    writeName(message, query.name());
    writeInteger<quint16>(message, query.type());
    writeInteger<quint16>(message, query.unicastResponse() ? 0x8001 : 1);
}

void DnsMessage::writeRecord(QByteArray &message, const DnsRecord &record) const
{
    writeName(message, record.name());
    writeInteger<quint16>(message, record.type());
    writeInteger<quint16>(message, record.flushCache() ? 0x8001 : 1);
    writeInteger<quint32>(message, record.ttl());
    writeInteger<quint16>(message, record.data().length());
    message.append(record.data());
}

void DnsMessage::writeName(QByteArray &message, const QByteArray &name) const
{
    foreach (QString label, name.split('.')) {
        writeInteger<quint8>(message, label.length());
        message.append(label.toUtf8());
    }
}

bool DnsMessage::parse(const QByteArray &message)
{
    quint16 offset = 0;
    quint16 transactionId, flags, nQueries, nAnswers, nAuthRecs, nAddRecs;
    if (!parseInteger<quint16>(message, offset, transactionId) ||
            !parseInteger<quint16>(message, offset, flags) ||
            !parseInteger<quint16>(message, offset, nQueries) ||
            !parseInteger<quint16>(message, offset, nAnswers) ||
            !parseInteger<quint16>(message, offset, nAuthRecs) ||
            !parseInteger<quint16>(message, offset, nAddRecs)) {
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
    if (!parseName(message, offset, name)) {
        return false;
    }
    quint16 type, class_;
    if (!parseInteger<quint16>(message, offset, type) ||
            !parseInteger<quint16>(message, offset, class_)) {
        return false;
    }
    mQueries.append(DnsQuery(name, type, class_ & 0x8000));
    return true;
}

bool DnsMessage::parseRecord(const QByteArray &message, quint16 &offset)
{
    QByteArray name;
    if (!parseName(message, offset, name)) {
        return false;
    }
    quint16 type, class_, nBytes;
    quint32 ttl;
    if (!parseInteger<quint16>(message, offset, type) ||
            !parseInteger<quint16>(message, offset, class_) ||
            !parseInteger<quint32>(message, offset, ttl) ||
            !parseInteger<quint16>(message, offset, nBytes) ||
            offset + nBytes > message.length()) {
        return false;
    }
    QByteArray data(message.constData() + offset, nBytes);
    offset += nBytes;
    mRecords.append(DnsRecord{name, type, class_ & 0x8000, ttl, data});
    return true;
}

bool DnsMessage::parseName(const QByteArray &message, quint16 &offset, QByteArray &name)
{
    quint16 offsetEnd = 0;
    quint16 offsetPtr = offset;
    forever {
        if (offset >= message.length()) {
            return false;  // out-of-bounds
        }
        quint8 nBytes;
        if (!parseInteger<quint8>(message, offset, nBytes)) {
            return false;
        }
        if (!nBytes) {
            break;
        }
        quint8 nBytes2;
        quint16 newOffset;
        switch (nBytes & 0xc0) {
        case 0x00:
            if (offset + nBytes > message.length()) {
                return false;  // length exceeds message
            }
            name.append(message.mid(offset, nBytes));
            name.append('.');
            offset += nBytes;
            break;
        case 0xc0:
            if (!parseInteger<quint8>(message, offset, nBytes2)) {
                return false;
            }
            newOffset = ((nBytes & ~0xc0) << 8) | nBytes2;
            if (newOffset >= offsetPtr) {
                return false;  // prevent infinite loop
            }
            offsetPtr = newOffset;
            if (!offsetEnd) {
                offsetEnd = offset;
            }
            offset = newOffset;
            break;
        default:
            return false;  // no other types supported
        }
    }
    if (offsetEnd) {
        offset = offsetEnd;
    }
    return true;
}
