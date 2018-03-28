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

#include <QDebug>

#include <qmdnsengine/dns.h>
#include <qmdnsengine/query.h>

#include "query_p.h"

using namespace QMdnsEngine;

QueryPrivate::QueryPrivate()
    : type(0),
      unicastResponse(false)
{
}

Query::Query()
    : d(new QueryPrivate)
{
}

Query::Query(const Query &other)
    : d(new QueryPrivate)
{
    *this = other;
}

Query &Query::operator=(const Query &other)
{
    *d = *other.d;
    return *this;
}

Query::~Query()
{
    delete d;
}

QByteArray Query::name() const
{
    return d->name;
}

void Query::setName(const QByteArray &name)
{
    d->name = name;
}

quint16 Query::type() const
{
    return d->type;
}

void Query::setType(quint16 type)
{
    d->type = type;
}

bool Query::unicastResponse() const
{
    return d->unicastResponse;
}

void Query::setUnicastResponse(bool unicastResponse)
{
    d->unicastResponse = unicastResponse;
}

QDebug QMdnsEngine::operator<<(QDebug dbg, const Query &query)
{
    QDebugStateSaver saver(dbg);
    Q_UNUSED(saver);

    dbg.noquote().nospace() << "Query(" << typeName(query.type()) << " " << query.name() << ")";

    return dbg;
}
