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

#include <QHostAddress>
#include <QtEndian>

#include <qmdnsengine/bitmap.h>
#include <qmdnsengine/dns.h>
#include <qmdnsengine/message.h>
#include <qmdnsengine/query.h>
#include <qmdnsengine/record.h>

namespace QMdnsEngine
{

template<class T>
bool parseInteger(const QByteArray &packet, quint16 &offset, T &value)
{
    if (offset + sizeof(T) > static_cast<unsigned int>(packet.length())) {
        return false;  // out-of-bounds
    }
    value = qFromBigEndian<T>(reinterpret_cast<const uchar*>(packet.constData() + offset));
    offset += sizeof(T);
    return true;
}

template<class T>
void writeInteger(QByteArray &packet, quint16 &offset, T value)
{
    value = qToBigEndian<T>(value);
    packet.append(reinterpret_cast<const char*>(&value), sizeof(T));
    offset += sizeof(T);
}

bool parseName(const QByteArray &packet, quint16 &offset, QByteArray &name)
{
    quint16 offsetEnd = 0;
    quint16 offsetPtr = offset;
    forever {
        quint8 nBytes;
        if (!parseInteger<quint8>(packet, offset, nBytes)) {
            return false;
        }
        if (!nBytes) {
            break;
        }
        switch (nBytes & 0xc0) {
        case 0x00:
            if (offset + nBytes > packet.length()) {
                return false;  // length exceeds message
            }
            name.append(packet.mid(offset, nBytes));
            name.append('.');
            offset += nBytes;
            break;
        case 0xc0:
        {
            quint8 nBytes2;
            quint16 newOffset;
            if (!parseInteger<quint8>(packet, offset, nBytes2)) {
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
        }
        default:
            return false;  // no other types supported
        }
    }
    if (offsetEnd) {
        offset = offsetEnd;
    }
    return true;
}

void writeName(QByteArray &packet, quint16 &offset, const QByteArray &name, QMap<QByteArray, quint16> &nameMap)
{
    QByteArray fragment = name;
    if (fragment.endsWith('.')) {
        fragment.chop(1);
    }
    while (fragment.length()) {
        if (nameMap.contains(fragment)) {
            writeInteger<quint16>(packet, offset, nameMap.value(fragment) | 0xc000);
            return;
        }
        nameMap.insert(fragment, offset);
        int index = fragment.indexOf('.');
        if (index == -1) {
            index = fragment.length();
        }
        writeInteger<quint8>(packet, offset, index);
        packet.append(fragment.left(index));
        offset += index;
        fragment.remove(0, index + 1);
    }
    writeInteger<quint8>(packet, offset, 0);
}

bool parseRecord(const QByteArray &packet, quint16 &offset, Record &record)
{
    QByteArray name;
    quint16 type, class_, dataLen;
    quint32 ttl;
    if (!parseName(packet, offset, name) ||
            !parseInteger<quint16>(packet, offset, type) ||
            !parseInteger<quint16>(packet, offset, class_) ||
            !parseInteger<quint32>(packet, offset, ttl) ||
            !parseInteger<quint16>(packet, offset, dataLen)) {
        return false;
    }
    record.setName(name);
    record.setType(type);
    record.setFlushCache(class_ & 0x8000);
    record.setTtl(ttl);
    switch (type) {
    case A:
    {
        quint32 ipv4Addr;
        if (!parseInteger<quint32>(packet, offset, ipv4Addr)) {
            return false;
        }
        record.setAddress(QHostAddress(ipv4Addr));
        break;
    }
    case AAAA:
    {
        if (offset + 16 > packet.length()) {
            return false;
        }
        record.setAddress(QHostAddress(
            reinterpret_cast<const quint8*>(packet.constData() + offset)
        ));
        offset += 16;
        break;
    }
    case NSEC:
    {
        QByteArray nextDomainName;
        quint8 number;
        quint8 length;
        if (!parseName(packet, offset, nextDomainName) ||
                !parseInteger<quint8>(packet, offset, number) ||
                !parseInteger<quint8>(packet, offset, length) ||
                number != 0 ||
                offset + length > packet.length()) {
            return false;
        }
        Bitmap bitmap;
        bitmap.setData(length, reinterpret_cast<const quint8*>(packet.constData() + offset));
        record.setNextDomainName(nextDomainName);
        record.setBitmap(bitmap);
        offset += length;
        break;
    }
    case PTR:
    {
        QByteArray target;
        if (!parseName(packet, offset, target)) {
            return false;
        }
        record.setTarget(target);
        break;
    }
    case SRV:
    {
        quint16 priority, weight, port;
        QByteArray target;
        if (!parseInteger<quint16>(packet, offset, priority) ||
                !parseInteger<quint16>(packet, offset, weight) ||
                !parseInteger<quint16>(packet, offset, port) ||
                !parseName(packet, offset, target)) {
            return false;
        }
        record.setPriority(priority);
        record.setWeight(weight);
        record.setPort(port);
        record.setTarget(target);
        break;
    }
    case TXT:
    {
        quint16 start = offset;
        while (offset < start + dataLen) {
            quint8 nBytes;
            if (!parseInteger<quint8>(packet, offset, nBytes) ||
                    offset + nBytes > packet.length()) {
                return false;
            }
            if (nBytes == 0) {
                break;
            }
            QByteArray attr(packet.constData() + offset, nBytes);
            offset += nBytes;
            int splitIndex = attr.indexOf('=');
            if (splitIndex == -1) {
                record.addAttribute(attr, QByteArray());
            } else {
                record.addAttribute(attr.left(splitIndex), attr.mid(splitIndex + 1));
            }
        }
        break;
    }
    default:
        offset += dataLen;
        break;
    }
    return true;
}

void writeRecord(QByteArray &packet, quint16 &offset, Record &record, QMap<QByteArray, quint16> &nameMap)
{
    writeName(packet, offset, record.name(), nameMap);
    writeInteger<quint16>(packet, offset, record.type());
    writeInteger<quint16>(packet, offset, record.flushCache() ? 0x8001 : 1);
    writeInteger<quint32>(packet, offset, record.ttl());
    offset += 2;
    QByteArray data;
    switch (record.type()) {
    case A:
        writeInteger<quint32>(data, offset, record.address().toIPv4Address());
        break;
    case AAAA:
    {
        Q_IPV6ADDR ipv6Addr = record.address().toIPv6Address();
        data.append(reinterpret_cast<const char*>(&ipv6Addr), sizeof(Q_IPV6ADDR));
        offset += data.length();
        break;
    }
    case NSEC:
    {
        quint8 length = record.bitmap().length();
        writeName(data, offset, record.nextDomainName(), nameMap);
        writeInteger<quint8>(data, offset, 0);
        writeInteger<quint8>(data, offset, length);
        data.append(reinterpret_cast<const char*>(record.bitmap().data()), length);
        offset += length;
        break;
    }
    case PTR:
        writeName(data, offset, record.target(), nameMap);
        break;
    case SRV:
        writeInteger<quint16>(data, offset, record.priority());
        writeInteger<quint16>(data, offset, record.weight());
        writeInteger<quint16>(data, offset, record.port());
        writeName(data, offset, record.target(), nameMap);
        break;
    case TXT:
        if (!record.attributes().count()) {
            writeInteger<quint8>(data, offset, 0);
            break;
        }
        for (auto i = record.attributes().constBegin(); i != record.attributes().constEnd(); ++i) {
            QByteArray entry = i.value().isNull() ? i.key() : i.key() + "=" + i.value();
            writeInteger<quint8>(data, offset, entry.length());
            data.append(entry);
            offset += entry.length();
        }
        break;
    default:
        break;
    }
    offset -= 2;
    writeInteger<quint16>(packet, offset, data.length());
    packet.append(data);
}

bool fromPacket(const QByteArray &packet, Message &message)
{
    quint16 offset = 0;
    quint16 transactionId, flags, nQuestion, nAnswer, nAuthority, nAdditional;
    if (!parseInteger<quint16>(packet, offset, transactionId) ||
            !parseInteger<quint16>(packet, offset, flags) ||
            !parseInteger<quint16>(packet, offset, nQuestion) ||
            !parseInteger<quint16>(packet, offset, nAnswer) ||
            !parseInteger<quint16>(packet, offset, nAuthority) ||
            !parseInteger<quint16>(packet, offset, nAdditional)) {
        return false;
    }
    message.setTransactionId(transactionId);
    message.setResponse(flags & 0x8400);
    for (int i = 0; i < nQuestion; ++i) {
        QByteArray name;
        quint16 type, class_;
        if (!parseName(packet, offset, name) ||
                !parseInteger<quint16>(packet, offset, type) ||
                !parseInteger<quint16>(packet, offset, class_)) {
            return false;
        }
        Query query;
        query.setName(name);
        query.setType(type);
        query.setUnicastResponse(class_ & 0x8000);
        message.addQuery(query);
    }
    quint16 nRecord = nAnswer + nAuthority + nAdditional;
    for (int i = 0; i < nRecord; ++i) {
        Record record;
        if (!parseRecord(packet, offset, record)) {
            return false;
        }
        message.addRecord(record);
    }
    return true;
}

void toPacket(const Message &message, QByteArray &packet)
{
    quint16 offset = 0;
    writeInteger<quint16>(packet, offset, message.transactionId());
    writeInteger<quint16>(packet, offset, message.isResponse() ? 0x8400 : 0);
    writeInteger<quint16>(packet, offset, message.queries().length());
    writeInteger<quint16>(packet, offset, message.records().length());
    writeInteger<quint16>(packet, offset, 0);
    writeInteger<quint16>(packet, offset, 0);
    QMap<QByteArray, quint16> nameMap;
    foreach (Query query, message.queries()) {
        writeName(packet, offset, query.name(), nameMap);
        writeInteger<quint16>(packet, offset, query.type());
        writeInteger<quint16>(packet, offset, query.unicastResponse() ? 0x8001 : 1);
    }
    foreach (Record record, message.records()) {
        writeRecord(packet, offset, record, nameMap);
    }
}

QString typeName(quint16 type)
{
    switch (type) {
    case A:    return "A";
    case AAAA: return "AAAA";
    case ANY:  return "ANY";
    case NSEC: return "NSEC";
    case PTR:  return "PTR";
    case SRV:  return "SRV";
    case TXT:  return "TXT";
    default:   return "?";
    }
}

}
