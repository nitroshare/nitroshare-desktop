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

#ifndef MDNSMESSAGE_H
#define MDNSMESSAGE_H

#include <QHostAddress>
#include <QList>

#include "mdns.h"
#include "mdnsquery.h"
#include "mdnsrecord.h"

/**
 * @brief DNS message
 *
 * Messages consist of queries and records.
 */
class MdnsMessage
{
public:

    /**
     * @brief Create an empty DNS message
     */
    MdnsMessage();

    /**
     * @brief Retrieve message address
     */
    QHostAddress address() const;

    /**
     * @brief Set message address
     */
    void setAddress(const QHostAddress &address);

    /**
     * @brief Retrieve message protocol
     */
    Mdns::Protocol protocol() const;

    /**
     * @brief Set message protocol
     */
    void setProtocol(Mdns::Protocol protocol);

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
    QList<MdnsQuery> queries() const;

    /**
     * @brief Add a query to the message
     */
    void addQuery(const MdnsQuery &query);

    /**
     * @brief Retrieve the records in the message
     */
    QList<MdnsRecord> records() const;

    /**
     * @brief Add a record to the message
     */
    void addRecord(const MdnsRecord &record);

    /**
     * @brief Create a message pre-initialized to reply to this one
     */
    MdnsMessage reply() const;

private:

    QHostAddress mAddress;
    Mdns::Protocol mProtocol;
    quint16 mPort;

    quint16 mTransactionId;
    bool mResponse;
    QList<MdnsQuery> mQueries;
    QList<MdnsRecord> mRecords;
};

#endif // MDNSMESSAGE_H
