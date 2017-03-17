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

    /**
     * @brief Retrieve the record name
     */
    QByteArray name() const;

    /**
     * @brief Set the record name
     */
    void setName(const QByteArray &name);

    /**
     * @brief Retrieve the record type
     */
    quint16 type() const;

    /**
     * @brief Set the record type
     */
    void setType(quint16 type);

    /**
     * @brief Retrieve the cache flush state
     */
    bool flushCache() const;

    /**
     * @brief Set the cache flush state
     */
    void setFlushCache(bool flushCache);

    /**
     * @brief Retrieve the record TTL
     */
    quint32 ttl() const;

    /**
     * @brief Set the record TTL
     */
    void setTtl(quint32 ttl);

    /**
     * @brief Retrieve the record address
     */
    QHostAddress address() const;

    /**
     * @brief Set the record address
     */
    void setAddress(const QHostAddress &address);

    /**
     * @brief Retrieve the record target
     */
    QByteArray target() const;

    /**
     * @brief Set the record target
     */
    void setTarget(const QByteArray &target);

    /**
     * @brief Retrieve the record priority
     */
    quint16 priority() const;

    /**
     * @brief Set the record priority
     */
    void setPriority(quint16 priority);

    /**
     * @brief Retrieve the record weight
     */
    quint16 weight() const;

    /**
     * @brief Set the record weight
     */
    void setWeight(quint16 weight);

    /**
     * @brief Retrieve the record port
     */
    quint16 port() const;

    /**
     * @brief Set the record port
     */
    void setPort(quint16 port);

    /**
     * @brief Retrieve record TXT values
     */
    QMap<QByteArray, QByteArray> txt();

    /**
     * @brief Add record TXT value
     */
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
