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
#include <QMap>

/**
 * @brief DNS record
 */
class DnsRecord
{
public:

    DnsRecord(const QByteArray &name, quint16 type, bool flushCache, quint32 ttl, const QByteArray &data = QByteArray());

    QByteArray name() const;
    quint16 type() const;
    bool flushCache() const;
    quint32 ttl() const;
    QByteArray data() const;

    QMap<QByteArray, QByteArray> txt() const;
    void setTxt(const QMap<QByteArray, QByteArray> txt);

private:

    QByteArray mName;
    quint16 mType;
    bool mFlushCache;
    quint32 mTtl;
    QByteArray mData;
};

#endif // DNSRECORD_H
