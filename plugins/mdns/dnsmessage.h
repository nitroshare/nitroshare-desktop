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

#ifndef DNSMESSAGE_H
#define DNSMESSAGE_H

#include <QByteArray>
#include <QList>
#include <QtEndian>

/**
 * @brief Incredibly simple DNS message parser and generator
 */
class DnsMessage
{
public:

    enum {
        A = 1,
        AAAA = 28,
        PTR = 12,
        SRV = 33,
        TXT = 16
    };

    struct Query
    {
        QByteArray name;
        quint16 type;
        bool unicast;
    };

    struct Record
    {
        QByteArray name;
        quint16 type;
        bool flush;
        quint32 ttl;
        QByteArray data;
    };

    DnsMessage();
    explicit DnsMessage(const QByteArray &message);

    bool isOkay() const;

    QList<Query> queries() const;
    QList<Record> records() const;

    void addQuery(const Query &query);
    void addRecord(const Record &record);

    QByteArray toMessage() const;

private:

    void writeQuery(QByteArray &message, const Query &query) const;
    void writeRecord(QByteArray &message, const Record &answer) const;
    void writeName(QByteArray &message, const QByteArray &name) const;

    bool parse(const QByteArray &message);
    bool parseQuery(const QByteArray &message, quint16 &offset);
    bool parseRecord(const QByteArray &message, quint16 &offset);
    bool parseName(const QByteArray &message, quint16 &offset, QByteArray &name);

    template<class T>
    void writeInteger(QByteArray &message, T value) const
    {
        value = qToBigEndian<T>(value);
        message.append(reinterpret_cast<const char*>(&value), sizeof(T));
    }

    template<class T>
    bool parseInteger(const QByteArray &message, quint16 &offset, T &value)
    {
        if (offset + sizeof(T) > message.length()) {
            return false;  // out-of-bounds
        }
        value = qFromBigEndian<T>(reinterpret_cast<const uchar*>(message.constData() + offset));
        offset += sizeof(T);
        return true;
    }

    bool mOkay;

    QList<Query> mQueries;
    QList<Record> mRecords;
};

#endif // DNSMESSAGE_H

