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

#include "dnsquery.h"
#include "dnsrecord.h"
#include "dnsutil.h"

// TODO: check into proper escaping for TXT records

bool DnsUtil::fromPacket(const QByteArray &packet, DnsMessage &message)
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
        DnsQuery query;
        query.setName(name);
        query.setType(type);
        query.setUnicastResponse(class_ & 0x8000);
        message.addQuery(query);
    }
    quint16 nRecord = nAnswer + nAuthority + nAdditional;
    for (int i = 0; i < nRecord; ++i) {
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
        DnsRecord record;
        record.setName(name);
        record.setType(type);
        record.setFlushCache(class_ & 0x8000);
        record.setTtl(ttl);
        switch (type) {
        case DnsMessage::A:
        {
            quint32 ipv4Addr;
            if (!parseInteger<quint32>(packet, offset, ipv4Addr)) {
                return false;
            }
            record.setAddress(QHostAddress(ipv4Addr));
            break;
        }
        case DnsMessage::AAAA:
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
        case DnsMessage::PTR:
        {
            QByteArray target;
            if (!parseName(packet, offset, target)) {
                return false;
            }
            record.setTarget(target);
            break;
        }
        case DnsMessage::SRV:
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
        case DnsMessage::TXT:
        {
            quint16 start = offset;
            while (offset < start + dataLen) {
                quint8 nBytes;
                if (!parseInteger<quint8>(packet, offset, nBytes) ||
                        offset + nBytes > packet.length()) {
                    return false;
                }
                QByteArray entry(packet.constData() + offset, nBytes);
                offset += nBytes;
                int splitIndex = entry.indexOf('=');
                if (splitIndex == -1) {
                    return false;
                }
                record.addTxt(entry.left(splitIndex), entry.mid(splitIndex + 1));
            }
            break;
        }
        default:
            offset += dataLen;
            break;
        }
        message.addRecord(record);
    }
    return true;
}

void DnsUtil::toPacket(const DnsMessage &message, QByteArray &packet)
{
    quint16 offset = 0;
    writeInteger<quint16>(packet, offset, message.transactionId());
    writeInteger<quint16>(packet, offset, message.isResponse() ? 0x8400 : 0);
    writeInteger<quint16>(packet, offset, message.queries().length());
    writeInteger<quint16>(packet, offset, message.records().length());
    writeInteger<quint16>(packet, offset, 0);
    writeInteger<quint16>(packet, offset, 0);
    QMap<QByteArray, quint16> nameMap;
    foreach (DnsQuery query, message.queries()) {
        writeName(packet, offset, query.name(), nameMap);
        writeInteger<quint16>(packet, offset, query.type());
        writeInteger<quint16>(packet, offset, query.unicastResponse() ? 0x8001 : 1);
    }
    foreach (DnsRecord record, message.records()) {
        writeName(packet, offset, record.name(), nameMap);
        writeInteger<quint16>(packet, offset, record.type());
        writeInteger<quint16>(packet, offset, record.flushCache() ? 0x8001 : 1);
        writeInteger<quint32>(packet, offset, record.ttl());
        QByteArray data;
        switch (record.type()) {
        case DnsMessage::A:
            writeInteger<quint32>(data, offset, record.address().toIPv4Address());
            break;
        case DnsMessage::AAAA:
        {
            Q_IPV6ADDR ipv6Addr = record.address().toIPv6Address();
            data.append(reinterpret_cast<const char*>(&ipv6Addr), sizeof(Q_IPV6ADDR));
            break;
        }
        case DnsMessage::PTR:
            writeName(data, offset, record.target(), nameMap);
            break;
        case DnsMessage::SRV:
            writeInteger<quint16>(data, offset, record.priority());
            writeInteger<quint16>(data, offset, record.weight());
            writeInteger<quint16>(data, offset, record.port());
            writeName(data, offset, record.target(), nameMap);
            break;
        case DnsMessage::TXT:
            for (QMap<QByteArray, QByteArray>::const_iterator i = record.txt().constBegin();
                    i != record.txt().constEnd(); ++i) {
                QByteArray entry = i.key() + "=" + i.value();
                writeInteger<quint8>(data, offset, entry.length());
                data.append(entry);
            }
            break;
        default:
            break;
        }
        writeInteger<quint16>(packet, offset, data.length());
        packet.append(data);
        offset += data.length();
    }
}

bool DnsUtil::parseName(const QByteArray &packet, quint16 &offset, QByteArray &name)
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

void DnsUtil::writeName(QByteArray &packet, quint16 &offset, const QByteArray &name, QMap<QByteArray, quint16> &nameMap)
{
    QByteArray fragment = name;
    if (fragment.endsWith('.')) {
        fragment.left(fragment.length() - 1);
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
