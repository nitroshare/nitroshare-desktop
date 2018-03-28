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

#ifndef QMDNSENGINE_SERVICE_H
#define QMDNSENGINE_SERVICE_H

#include <QByteArray>
#include <QHostAddress>
#include <QList>
#include <QMap>

#include "qmdnsengine_export.h"

namespace QMdnsEngine
{

class QMDNSENGINE_EXPORT ServicePrivate;

/**
 * @brief %Service available on the local network
 *
 * This class contains the descriptive information necessary to represent an
 * individual service made available to the local network. Instances are
 * provided by [Browser](@ref QMdnsEngine::Browser) as services are
 * discovered. Instances must be created and passed to
 * [Provider::update()](@ref QMdnsEngine::Provider::update) to provide a
 * service.
 */
class QMDNSENGINE_EXPORT Service
{
public:

    /**
     * @brief Create an uninitialized service
     */
    Service();

    /**
     * @brief Create a copy of an existing service
     */
    Service(const Service &other);

    /**
     * @brief Assignment operator
     */
    Service &operator=(const Service &other);

    /**
     * @brief Equality operator
     */
    bool operator==(const Service &other) const;

    /**
     * @brief Inequality operator
     */
    bool operator!=(const Service &other) const;

    /**
     * @brief Destroy the service
     */
    virtual ~Service();

    /**
     * @brief Retrieve the service type
     */
    QByteArray type() const;

    /**
     * @brief Set the service type
     *
     * For example, an HTTP service might use "_http._tcp".
     */
    void setType(const QByteArray &type);

    /**
     * @brief Retrieve the service name
     */
    QByteArray name() const;

    /**
     * @brief Set the service name
     *
     * This is combined with the service type and domain to form the FQDN for
     * the service.
     */
    void setName(const QByteArray &name);

    /**
     * @brief Retrieve the hostname of the device providing the service
     */
    QByteArray hostname() const;

    /**
     * @brief Set the hostname of the device providing the service
     */
    void setHostname(const QByteArray &hostname);

    /**
     * @brief Retrieve the service port
     */
    quint16 port() const;

    /**
     * @brief Set the service port
     */
    void setPort(quint16 port);

    /**
     * @brief Retrieve the attributes for the service
     *
     * Boolean attributes will have null values (invoking QByteArray::isNull()
     * on the value will return true).
     */
    QMap<QByteArray, QByteArray> attributes() const;

    /**
     * @brief Set the attributes for the service
     */
    void setAttributes(const QMap<QByteArray, QByteArray> &attributes);

    /**
     * @brief Add an attribute to the service
     */
    void addAttribute(const QByteArray &key, const QByteArray &value);

private:

    ServicePrivate *const d;
};

}

#endif // QMDNSENGINE_SERVICE_H
