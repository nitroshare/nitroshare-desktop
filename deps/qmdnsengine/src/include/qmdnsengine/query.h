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

#ifndef QMDNSENGINE_QUERY_H
#define QMDNSENGINE_QUERY_H

#include <QByteArray>

#include "qmdnsengine_export.h"

namespace QMdnsEngine
{

class QMDNSENGINE_EXPORT QueryPrivate;

/**
 * @brief DNS query
 *
 * This class represents a query for a DNS record. For example, to query for
 * the IPv4 address of a local host:
 *
 * @code
 * QMdnsEngine::Query query;
 * query.setName("myserver.local.");
 * query.setType(QMdnsEngine::A);
 *
 * message.addQuery(query);
 * @endcode
 */
class QMDNSENGINE_EXPORT Query
{
public:

    /**
     * @brief Create an empty query
     */
    Query();

    /**
     * @brief Create a copy of an existing query
     */
    Query(const Query &other);

    /**
     * @brief Assignment operator
     */
    Query &operator=(const Query &other);

    /**
     * @brief Destroy the query
     */
    virtual ~Query();

    /**
     * @brief Retrieve the name being queried
     */
    QByteArray name() const;

    /**
     * @brief Set the name to query
     */
    void setName(const QByteArray &name);

    /**
     * @brief Retrieve the type of record being queried
     */
    quint16 type() const;

    /**
     * @brief Set the type of record to query
     *
     * Constants, such as QMdnsEngine::SRV are provided for convenience.
     */
    void setType(quint16 type);

    /**
     * @brief Determine if a unicast response is desired
     */
    bool unicastResponse() const;

    /**
     * @brief Set whether a unicast response is desired
     */
    void setUnicastResponse(bool unicastResponse);

private:

    QueryPrivate *const d;
};

QMDNSENGINE_EXPORT QDebug operator<<(QDebug dbg, const Query &query);

}

#endif // QMDNSENGINE_QUERY_H
