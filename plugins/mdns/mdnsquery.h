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

#ifndef MDNSQUERY_H
#define MDNSQUERY_H

#include <QByteArray>

/**
 * @brief DNS query
 */
class MdnsQuery
{
public:

    /**
     * @brief Create an empty mDNS query
     */
    MdnsQuery();

    /**
     * @brief Retrieve the query name
     */
    QByteArray name() const;

    /**
     * @brief Set the query name
     */
    void setName(const QByteArray &name);

    /**
     * @brief Retrieve the query type
     */
    quint16 type() const;

    /**
     * @brief Set the query type
     */
    void setType(quint16 type);

    /**
     * @brief Determine if the response should be unicast
     */
    bool unicastResponse() const;

    /**
     * @brief Set whether the response should be unicast
     */
    void setUnicastResponse(bool unicastResponse);

private:

    QByteArray mName;
    quint16 mType;
    bool mUnicastResponse;
};

#endif // MDNSQUERY_H
