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

#include <QHostAddress>
#include <QList>

#include "dnsquery.h"
#include "dnsrecord.h"

/**
 * @brief DNS message
 *
 * Messages consist of queries and records.
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

    /**
     * @brief Create an empty DNS message
     */
    DnsMessage();

    /**
     * @brief Retrieve message address
     */
    QHostAddress address() const;

    /**
     * @brief Set message address
     */
    void setAddress(const QHostAddress &address);

    /**
     * @brief Retrieve message port
     */
    quint16 port() const;

    /**
     * @brief Set message port
     */
    void setPort(quint16 port);

    /**
     * @brief Retrieve the transaction ID for the message
     */
    quint16 transactionId() const;

    /**
     * @brief Set the transaction ID for the message
     */
    void setTransactionId(quint16 transactionId);

    /**
     * @brief Determine if the message is a response
     */
    bool isResponse() const;

    /**
     * @brief Set whether the message is a response
     */
    void setResponse(bool response);

    /**
     * @brief Retrieve the queries in the message
     */
    QList<DnsQuery> queries() const;

    /**
     * @brief Add a query to the message
     */
    void addQuery(const DnsQuery &query);

    /**
     * @brief Retrieve the records in the message
     */
    QList<DnsRecord> records() const;

    /**
     * @brief Add a record to the message
     */
    void addRecord(const DnsRecord &record);

private:

    QHostAddress mAddress;
    quint16 mPort;

    quint16 mTransactionId;
    bool mResponse;
    QList<DnsQuery> mQueries;
    QList<DnsRecord> mRecords;
};

#endif // DNSMESSAGE_H
