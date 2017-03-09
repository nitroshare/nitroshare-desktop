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

DnsRecord::DnsRecord(const QByteArray &name, quint16 type, bool flushCache, quint32 ttl, const QByteArray &data)
    : mName(name),
      mType(type),
      mFlushCache(flushCache),
      mTtl(ttl),
      mData(data)
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
    return mData;
}

QMap<QByteArray, QByteArray> DnsRecord::txt() const
{
    QMap<QByteArray, QByteArray> map;
    quint8 offset = 0;
    forever {
        if (offset >= mData.length()) {
            break;
        }
        quint8 nBytes = *reinterpret_cast<const quint8*>(mData.constData() + offset++);
        if (nBytes == 0 || offset + nBytes > mData.length()) {
            break;
        }
        QByteArray entry(mData.constData() + offset, nBytes);
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
        quint8 nBytes = entry.length();
        mData.append(reinterpret_cast<const char*>(&nBytes), sizeof(quint8));
        mData.append(entry);
    }
}
