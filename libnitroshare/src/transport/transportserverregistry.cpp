/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Nathan Osman
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

#include <nitroshare/device.h>
#include <nitroshare/transport.h>
#include <nitroshare/transportserver.h>
#include <nitroshare/transportserverregistry.h>

#include "transportserverregistry_p.h"

TransportServerRegistryPrivate::TransportServerRegistryPrivate(QObject *parent)
    : QObject(parent)
{
}

TransportServerRegistry::TransportServerRegistry(QObject *parent)
    : QObject(parent),
      d(new TransportServerRegistryPrivate(this))
{
}

void TransportServerRegistry::add(TransportServer *server)
{
    connect(server, &TransportServer::transportReceived, this, &TransportServerRegistry::transportReceived);
    d->transportServers.insert(server->name(), server);
}

void TransportServerRegistry::remove(TransportServer *server)
{
    disconnect(server, &TransportServer::transportReceived, this, &TransportServerRegistry::transportReceived);
    d->transportServers.remove(server->name());
}

Transport *TransportServerRegistry::createTransport(Device *device)
{
    TransportServer *transportServer = d->transportServers.value(device->transportName());
    if (!transportServer) {
        return nullptr;
    }
    return transportServer->createTransport(device);
}
