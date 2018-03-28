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

#ifndef QMDNSENGINE_PROVIDER_H
#define QMDNSENGINE_PROVIDER_H

#include <QObject>

#include "qmdnsengine_export.h"

namespace QMdnsEngine
{

class AbstractServer;
class Hostname;
class Service;

class QMDNSENGINE_EXPORT ProviderPrivate;

/**
 * @brief %Provider for a single mDNS service
 *
 * This class provide a [Service](@ref QMdnsEngine::Service) on the local
 * network by responding to the appropriate DNS queries. A hostname is
 * required for creating the SRV record.
 *
 * The provider needs to be given a reference to the service through the
 * update() method so that it can construct DNS records:
 *
 * @code
 * QMdnsEngine::Service service;
 * service.setType("_http._tcp.local.");
 * service.setName("My Service");
 * service.setPort(1234);
 *
 * QMdnsEngine::Provider provider;
 * provider.update(service);
 * @endcode
 *
 * This method can also be used to update the provider's records.
 */
class QMDNSENGINE_EXPORT Provider : public QObject
{
    Q_OBJECT

public:

    /**
     * @brief Create a new service provider
     */
    Provider(AbstractServer *server, Hostname *hostname, QObject *parent = 0);

    /**
     * @brief Update the service with the provided information
     * @param service updated service description
     *
     * This class will not respond to any DNS queries until the hostname is
     * confirmed and this method is called.
     */
    void update(const Service &service);

private:

    ProviderPrivate *const d;
};

}

#endif // QMDNSENGINE_PROVIDER_H
