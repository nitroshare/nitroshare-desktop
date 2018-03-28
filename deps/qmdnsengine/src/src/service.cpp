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

#include <qmdnsengine/service.h>

#include "service_p.h"

using namespace QMdnsEngine;

ServicePrivate::ServicePrivate()
{
}

Service::Service()
    : d(new ServicePrivate)
{
}

Service::Service(const Service &other)
    : d(new ServicePrivate)
{
    *this = other;
}

Service &Service::operator=(const Service &other)
{
    *d = *other.d;
    return *this;
}

bool Service::operator==(const Service &other) const
{
    return d->type == other.d->type &&
        d->name == other.d->name &&
        d->port == other.d->port &&
        d->attributes == other.d->attributes;
}

bool Service::operator!=(const Service &other) const
{
    return !(*this == other);
}

Service::~Service()
{
    delete d;
}

QByteArray Service::type() const
{
    return d->type;
}

void Service::setType(const QByteArray &type)
{
    d->type = type;
}

QByteArray Service::name() const
{
    return d->name;
}

void Service::setName(const QByteArray &name)
{
    d->name = name;
}

QByteArray Service::hostname() const
{
    return d->hostname;
}

void Service::setHostname(const QByteArray &hostname)
{
    d->hostname = hostname;
}

quint16 Service::port() const
{
    return d->port;
}

void Service::setPort(quint16 port)
{
    d->port = port;
}

QMap<QByteArray, QByteArray> Service::attributes() const
{
    return d->attributes;
}

void Service::setAttributes(const QMap<QByteArray, QByteArray> &attributes)
{
    d->attributes = attributes;
}

void Service::addAttribute(const QByteArray &key, const QByteArray &value)
{
    d->attributes.insert(key, value);
}
