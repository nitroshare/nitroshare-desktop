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

#ifndef DNSRECORD_H
#define DNSRECORD_H

#include <QByteArray>
#include <QHostAddress>
#include <QMap>

/**
 * @brief DNS record
 */
class DnsRecord
{
public:

    /**
     * @brief Create an empty DNS record
     */
    DnsRecord();

    QByteArray name() const;
    void setName(const QByteArray &name);
    quint16 type() const;
    void setType(quint16 type);
    bool flushCache() const;
    void setFlushCache(bool flushCache);
    quint32 ttl() const;
    void setTtl(quint32 ttl);
    QHostAddress address() const;
    void setAddress(const QHostAddress &address);
    QByteArray target() const;
    void setTarget(const QByteArray &target);
    quint16 priority() const;
    void setPriority(quint16 priority);
    quint16 weight() const;
    void setWeight(quint16 weight);
    quint16 port() const;
    void setPort(quint16 port);
    QMap<QByteArray, QByteArray> txt();
    void addTxt(const QByteArray &key, const QByteArray &value);

private:

    QByteArray mName;
    quint16 mType;
    bool mFlushCache;
    quint32 mTtl;
    QHostAddress mAddress;
    QByteArray mTarget;
    quint16 mPriority;
    quint16 mWeight;
    quint16 mPort;
    QMap<QByteArray, QByteArray> mTxt;
};

#endif // DNSRECORD_H
