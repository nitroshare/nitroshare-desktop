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

#ifndef QMDNSENGINE_BROWSER_H
#define QMDNSENGINE_BROWSER_H

#include <QByteArray>
#include <QObject>

#include "qmdnsengine_export.h"

namespace QMdnsEngine
{

class AbstractServer;
class Cache;
class Service;

class QMDNSENGINE_EXPORT BrowserPrivate;

/**
 * @brief %Browser for local services
 *
 * This class provides a simple way to discover services on the local network.
 * A cache may be provided in the constructor to store records for future
 * queries.
 *
 * To browse for services of any type:
 *
 * @code
 * QMdnsEngine::Browser browser(&server, QMdnsEngine::MdnsBrowseType);
 * @endcode
 *
 * To browse for services of a specific type:
 *
 * @code
 * QMdnsEngine::Browser browser(&server, "_http._tcp.local.");
 * @endcode
 *
 * When a service is found, the serviceAdded() signal is emitted:
 *
 * @code
 * connect(&browser, &QMdnsEngine::Browser::serviceAdded, [](const QMdnsEngine::Service &service) {
 *     qDebug() << "Service added:" << service.name();
 * });
 * @endcode
 *
 * The serviceUpdated() and serviceRemoved() signals are emitted when services
 * are updated (their properties change) or are removed, respectively.
 */
class QMDNSENGINE_EXPORT Browser : public QObject
{
    Q_OBJECT

public:

    /**
     * @brief Create a new browser instance
     * @param server server to use for receiving and sending mDNS messages
     * @param type service type to browse for
     * @param cache DNS cache to use or null to create one
     * @param parent QObject
     */
    Browser(AbstractServer *server, const QByteArray &type, Cache *cache = 0, QObject *parent = 0);

Q_SIGNALS:

    /**
     * @brief Indicate that a new service has been added
     *
     * This signal is emitted when the PTR and SRV records for a service are
     * received. If TXT records are received later, the serviceUpdated()
     * signal will be emitted.
     */
    void serviceAdded(const Service &service);

    /**
     * @brief Indicate that the specified service was updated
     *
     * This signal is emitted when the SRV record for a service (identified by
     * its name and type) or a TXT record has changed.
     */
    void serviceUpdated(const Service &service);

    /**
     * @brief Indicate that the specified service was removed
     *
     * This signal is emitted when an essential record (PTR or SRV) is
     * expiring from the cache. This will also occur when an updated PTR or
     * SRV record is received with a TTL of 0.
     */
    void serviceRemoved(const Service &service);

private:

    BrowserPrivate *const d;
};

}

#endif // QMDNSENGINE_BROWSER_H
