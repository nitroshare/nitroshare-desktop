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

#include <QMapIterator>

#include "dnsrecord.h"
#include "dnsutil.h"

DnsRecord::DnsRecord(const QByteArray &name, quint16 type, bool flushCache, quint32 ttl, const QByteArray &message, quint16 offset, quint16 length)
    : mName(name),
      mType(type),
      mFlushCache(flushCache),
      mTtl(ttl),
      mMessage(message),
      mOffset(offset),
      mLength(length)
{
}

QByteArray DnsRecord::name() const
{
    return mName;
}

quint16 DnsRecord::type() const
{
    return mType;
}

bool DnsRecord::flushCache() const
{
    return mFlushCache;
}

quint32 DnsRecord::ttl() const
{
    return mTtl;
}

QByteArray DnsRecord::data() const
{
    return mMessage.mid(mOffset, mLength);
}

QHostAddress DnsRecord::a() const
{
    quint32 ip4Addr;
    quint16 offset = mOffset;
    if (DnsUtil::parseInteger<quint32>(mMessage, offset, ip4Addr)) {
        return QHostAddress(ip4Addr);
    } else {
        return QHostAddress();
    }
}

void DnsRecord::setA(const QHostAddress &address)
{
    quint32 ip4Addr = qToBigEndian<quint32>(address.toIPv4Address());
    DnsUtil::writeInteger<quint32>(mMessage, ip4Addr);
}

QHostAddress DnsRecord::aaaa() const
{
    if (mLength == 16) {
        return QHostAddress(reinterpret_cast<const quint8*>(mMessage.constData() + mOffset));
    } else {
        return QHostAddress();
    }
}

void DnsRecord::setAaaa(const QHostAddress &address)
{
    Q_IPV6ADDR ip6Addr = address.toIPv6Address();
    mMessage.append(reinterpret_cast<const char*>(&ip6Addr), sizeof(Q_IPV6ADDR));
}

QByteArray DnsRecord::ptr() const
{
    QByteArray name;
    quint16 offset = mOffset;
    DnsUtil::parseName(mMessage, offset, name);
    return name;
}

void DnsRecord::setPtr(const QByteArray &name)
{
    DnsUtil::writeName(mMessage, name);
}

DnsRecord::SrvData DnsRecord::srv() const
{
    DnsRecord::SrvData srvData{0, 0, 0};
    quint16 offset = mOffset;
    DnsUtil::parseInteger<quint16>(mMessage, offset, srvData.priority) &&
        DnsUtil::parseInteger<quint16>(mMessage, offset, srvData.weight) &&
        DnsUtil::parseInteger<quint16>(mMessage, offset, srvData.port) &&
        DnsUtil::parseName(mMessage, offset, srvData.target);
    return srvData;
}

void DnsRecord::setSrv(const SrvData &srvData)
{
    DnsUtil::writeInteger<quint16>(mMessage, srvData.priority);
    DnsUtil::writeInteger<quint16>(mMessage, srvData.weight);
    DnsUtil::writeInteger<quint16>(mMessage, srvData.port);
    DnsUtil::writeName(mMessage, srvData.target);
}

QMap<QByteArray, QByteArray> DnsRecord::txt() const
{
    QMap<QByteArray, QByteArray> map;
    quint16 offset = mOffset;
    forever {
        if (offset >= mMessage.length()) {
            break;
        }
        quint8 nBytes;
        if (!DnsUtil::parseInteger<quint8>(mMessage, offset, nBytes) ||
                offset + nBytes > mMessage.length()) {
            break;
        }
        QByteArray entry(mMessage.constData() + offset, nBytes);
        offset += nBytes;
        int splitIndex = entry.indexOf('=');
        if (splitIndex == -1) {
            break;
        }
        map.insert(entry.left(splitIndex), entry.mid(splitIndex + 1));
    }
    return map;
}

void DnsRecord::setTxt(const QMap<QByteArray, QByteArray> txt)
{
    QMapIterator<QByteArray, QByteArray> i(txt);
    while (i.hasNext()) {
        QByteArray entry = i.key() + "=" + i.value();
        DnsUtil::writeInteger<quint8>(mMessage, entry.length());
        mMessage.append(entry);
    }
}
