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

#include "mdnsdevice.h"

MdnsDevice::MdnsDevice(QMdnsEngine::Server *server,
                       QMdnsEngine::Cache *cache,
                       const QMdnsEngine::Service &service)
    : mUuid(service.attributes().value("uuid", service.name())),
      mName(service.name()),
      mResolver(server, service.hostname(), cache)
{
    connect(&mResolver, &QMdnsEngine::Resolver::resolved, this, &MdnsDevice::onResolved);
}

QString MdnsDevice::uuid() const
{
    return mUuid;
}

QString MdnsDevice::name() const
{
    return mName;
}

QStringList MdnsDevice::addresses() const
{
    return mAddresses;
}

quint16 MdnsDevice::port() const
{
    return mPort;
}

void MdnsDevice::update(const QMdnsEngine::Service &service)
{
    mPort = service.port();
}

void MdnsDevice::onResolved(const QHostAddress &address)
{
    mAddresses.append(address.toString());
}
