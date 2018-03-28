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

#ifndef QMDNSENGINE_MESSAGE_H
#define QMDNSENGINE_MESSAGE_H

#include <QHostAddress>
#include <QList>

#include "qmdnsengine_export.h"

namespace QMdnsEngine
{

class Query;
class Record;

class QMDNSENGINE_EXPORT MessagePrivate;

/**
 * @brief DNS message
 *
 * A DNS message consists of a header and zero or more queries and records.
 * Instances of this class are created and initialized by
 * [AbstractServer](@ref QMdnsEngine::AbstractServer) when messages are
 * received from the network.
 *
 * If a message is being constructed in reply to one received from the
 * network, the reply() method can be used to simplify initialization:
 *
 * @code
 * connect(&server, &QMdnsEngine::Server::messageReceived, [](const QMdnsEngine::Message &message) {
 *     QMdnsEngine::Message reply;
 *     reply.reply(message);
 *     server.sendMessage(reply);
 * });
 * @endcode
 */
class QMDNSENGINE_EXPORT Message
{
public:

    /**
     * @brief Create an empty message
     */
    Message();

    /**
     * @brief Create a copy of an existing message
     */
    Message(const Message &other);

    /**
     * @brief Assignment operator
     */
    Message &operator=(const Message &other);

    /**
     * @brief Destroy the message
     */
    virtual ~Message();

    /**
     * @brief Retrieve the address for the message
     *
     * When receiving messages, this is the address that the message was
     * received from.
     */
    QHostAddress address() const;

    /**
     * @brief Set the address for the message
     *
     * When sending messages, this is the address that the message will be
     * sent to. QMdnsEngine::MdnsIpv4Address and QMdnsEngine::MdnsIpv6Address
     * can be used for mDNS messages.
     */
    void setAddress(const QHostAddress &address);

    /**
     * @brief Retrieve the port for the message
     *
     * When receiving messages, this is the port that the message was received
     * from. For traditional queries, this will be an ephemeral port. For mDNS
     * queries, this will always equal QMdnsEngine::MdnsPort.
     */
    quint16 port() const;

    /**
     * @brief Set the port for the message
     *
     * When sending messages, this is the port that the message will be sent
     * to. This should be set to QMdnsEngine::MdnsPort unless the message is a
     * reply to a traditional DNS query.
     */
    void setPort(quint16 port);

    /**
     * @brief Retrieve the transaction ID for the message
     *
     * This is always set to 1 for mDNS messages. Traditional DNS queries may
     * set this to an arbitrary integer.
     */
    quint16 transactionId() const;

    /**
     * @brief Set the transaction ID for the message
     *
     * The default transaction ID is 0. This value should not be changed
     * unless responding to a traditional DNS query.
     */
    void setTransactionId(quint16 transactionId);

    /**
     * @brief Determine if the message is a response
     */
    bool isResponse() const;

    /**
     * @brief Set whether the message is a response
     */
    void setResponse(bool isResponse);

    /**
     * @brief Retrieve a list of queries in the message
     */
    QList<Query> queries() const;

    /**
     * @brief Add a query to the message
     */
    void addQuery(const Query &query);

    /**
     * @brief Retrieve a list of records in the message
     */
    QList<Record> records() const;

    /**
     * @brief Add a record to the message
     */
    void addRecord(const Record &record);

    /**
     * @brief Reply to another message
     *
     * The message will be correctly initialized to respond to the other
     * message. This includes setting the target address, port, and
     * transaction ID.
     */
    void reply(const Message &other);

private:

    MessagePrivate *const d;
};

}

#endif // QMDNSENGINE_MESSAGE_H
