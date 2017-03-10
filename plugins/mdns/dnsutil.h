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

#ifndef DNSUTIL_H
#define DNSUTIL_H

#include <QByteArray>
#include <QtEndian>

/**
 * @brief Utility methods for working with DNS records
 */
class DnsUtil
{
public:

    template<class T>
    static void writeInteger(QByteArray &message, T value)
    {
        value = qToBigEndian<T>(value);
        message.append(reinterpret_cast<const char*>(&value), sizeof(T));
    }

    template<class T>
    static bool parseInteger(const QByteArray &message, quint16 &offset, T &value)
    {
        if (offset + sizeof(T) > message.length()) {
            return false;  // out-of-bounds
        }
        value = qFromBigEndian<T>(reinterpret_cast<const uchar*>(message.constData() + offset));
        offset += sizeof(T);
        return true;
    }

    static void writeName(QByteArray &data, const QByteArray &name);
    static bool parseName(const QByteArray &data, quint16 &offset, QByteArray &name);
};

#endif // DNSUTIL_H
