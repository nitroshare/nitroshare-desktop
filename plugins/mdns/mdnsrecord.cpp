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

#include "mdnsrecord.h"

MdnsRecord::MdnsRecord()
    : mType(0),
      mFlushCache(false),
      mTtl(0),
      mPriority(0),
      mWeight(0),
      mPort(0),
      mBitmap()
{
}

QByteArray MdnsRecord::name() const
{
    return mName;
}

void MdnsRecord::setName(const QByteArray &name)
{
    mName = name;
}

quint16 MdnsRecord::type() const
{
    return mType;
}

void MdnsRecord::setType(quint16 type)
{
    mType = type;
}

bool MdnsRecord::flushCache() const
{
    return mFlushCache;
}

void MdnsRecord::setFlushCache(bool flushCache)
{
    mFlushCache = flushCache;
}

quint32 MdnsRecord::ttl() const
{
    return mTtl;
}

void MdnsRecord::setTtl(quint32 ttl)
{
    mTtl = ttl;
}

QHostAddress MdnsRecord::address() const
{
    return mAddress;
}

void MdnsRecord::setAddress(const QHostAddress &address)
{
    mAddress = address;
}

QByteArray MdnsRecord::target() const
{
    return mName2;
}

void MdnsRecord::setTarget(const QByteArray &target)
{
    mName2 = target;
}

QByteArray MdnsRecord::nextDomainName() const
{
    return mName2;
}

void MdnsRecord::setNextDomainName(const QByteArray &nextDomainName)
{
    mName2 = nextDomainName;
}

quint16 MdnsRecord::priority() const
{
    return mPriority;
}

void MdnsRecord::setPriority(quint16 priority)
{
    mPriority = priority;
}

quint16 MdnsRecord::weight() const
{
    return mWeight;
}

void MdnsRecord::setWeight(quint16 weight)
{
    mWeight = weight;
}

quint16 MdnsRecord::port() const
{
    return mPort;
}

void MdnsRecord::setPort(quint16 port)
{
    mPort = port;
}

QMap<QByteArray, QByteArray> MdnsRecord::txt()
{
    return mTxt;
}

void MdnsRecord::addTxt(const QByteArray &key, const QByteArray &value)
{
    mTxt.insert(key, value);
}

void MdnsRecord::setTxt(const QMap<QByteArray, QByteArray> &txt)
{
    mTxt = txt;
}

MdnsBitmap MdnsRecord::bitmap() const
{
    return mBitmap;
}

void MdnsRecord::setBitmap(const MdnsBitmap &bitmap)
{
    mBitmap = bitmap;
}
