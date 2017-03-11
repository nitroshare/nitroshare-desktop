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

#include "dnsrecord.h"

DnsRecord::DnsRecord()
    : mType(0),
      mFlushCache(false),
      mTtl(0),
      mPriority(0),
      mWeight(0),
      mPort(0)
{
}

QByteArray DnsRecord::name() const
{
    return mName;
}

void DnsRecord::setName(const QByteArray &name)
{
    mName = name;
}

quint16 DnsRecord::type() const
{
    return mType;
}

void DnsRecord::setType(quint16 type)
{
    mType = type;
}

bool DnsRecord::flushCache() const
{
    return mFlushCache;
}

void DnsRecord::setFlushCache(bool flushCache)
{
    mFlushCache = flushCache;
}

quint32 DnsRecord::ttl() const
{
    return mTtl;
}

void DnsRecord::setTtl(quint32 ttl)
{
    mTtl = ttl;
}

QHostAddress DnsRecord::address() const
{
    return mAddress;
}

void DnsRecord::setAddress(const QHostAddress &address)
{
    mAddress = address;
}

QByteArray DnsRecord::target() const
{
    return mTarget;
}

void DnsRecord::setTarget(const QByteArray &target)
{
    mTarget = target;
}

quint16 DnsRecord::priority() const
{
    return mPriority;
}

void DnsRecord::setPriority(quint16 priority)
{
    mPriority = priority;
}

quint16 DnsRecord::weight() const
{
    return mWeight;
}

void DnsRecord::setWeight(quint16 weight)
{
    mWeight = weight;
}

quint16 DnsRecord::port() const
{
    return mPort;
}

void DnsRecord::setPort(quint16 port)
{
    mPort = port;
}

QMap<QByteArray, QByteArray> DnsRecord::txt()
{
    return mTxt;
}

void DnsRecord::addTxt(const QByteArray &key, const QByteArray &value)
{
    mTxt.insert(key, value);
}

