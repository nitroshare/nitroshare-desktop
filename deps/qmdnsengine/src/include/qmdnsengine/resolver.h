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

#ifndef QMDNSENGINE_RESOLVER_H
#define QMDNSENGINE_RESOLVER_H

#include <QHostAddress>
#include <QObject>

#include "qmdnsengine_export.h"

namespace QMdnsEngine
{

class AbstractServer;
class Cache;

class QMDNSENGINE_EXPORT ResolverPrivate;

/**
 * @brief %Resolver for services
 *
 * When [Browser](@ref QMdnsEngine::Browser) indicates that a new service has
 * been found, it becomes necessary to resolve the service in order to connect
 * to it. This class serves that role. A [Cache](@ref QMdnsEngine::Cache) can
 * optionally be provided to speed up the resolving process.
 *
 * For example, assuming that `record` is a SRV record:
 *
 * @code
 * QMdnsEngine::Resolver resolver(&server, record.target());
 * connect(&resolver, &QMdnsEngine::Resolver::resolved, [](const QHostAddress &address) {
 *     qDebug() << "Address:" << address;
 * });
 * @endcode
 */
class QMDNSENGINE_EXPORT Resolver : public QObject
{
    Q_OBJECT

public:

    /**
     * @brief Create a new resolver
     */
    Resolver(AbstractServer *server, const QByteArray &name, Cache *cache = 0, QObject *parent = 0);

Q_SIGNALS:

    /**
     * @brief Indicate that the host resolved to an address
     * @param address service address
     *
     * This signal will be emitted once for each resolved address. For
     * example, if a host provides both A and AAAA records, this signal will
     * be emitted twice.
     */
    void resolved(const QHostAddress &address);

private:

    ResolverPrivate *const d;
};

}

#endif // QMDNSENGINE_RESOLVER_H
