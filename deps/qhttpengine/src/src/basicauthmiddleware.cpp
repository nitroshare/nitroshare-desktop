/*
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

#include <qhttpengine/basicauthmiddleware.h>
#include <qhttpengine/ibytearray.h>
#include <qhttpengine/parser.h>
#include <qhttpengine/socket.h>

#include "basicauthmiddleware_p.h"

using namespace QHttpEngine;

BasicAuthMiddlewarePrivate::BasicAuthMiddlewarePrivate(QObject *parent, const QString &realm)
    : QObject(parent),
      realm(realm)
{
}

BasicAuthMiddleware::BasicAuthMiddleware(const QString &realm, QObject *parent)
    : Middleware(parent),
      d(new BasicAuthMiddlewarePrivate(this, realm))
{
}

void BasicAuthMiddleware::add(const QString &username, const QString &password)
{
    d->map.insert(username, password);
}

bool BasicAuthMiddleware::verify(const QString &username, const QString &password)
{
    return d->map.contains(username) && d->map.value(username) == password;
}

bool BasicAuthMiddleware::process(Socket *socket)
{
    // Attempt to extract credentials from the header
    QByteArrayList headerParts = socket->headers().value("Authorization").split(' ');
    if (headerParts.count() == 2 && headerParts.at(0) == IByteArray("Basic")) {

        // Decode the credentials and split into username/password
        QByteArrayList parts;
        Parser::split(
            QByteArray::fromBase64(headerParts.at(1)),
            ":", 1, parts
        );

        // Verify credentials
        if (parts.count() == 2 && verify(parts.at(0), parts.at(1))) {
            return true;
        }
    }

    // Otherwise, inform the client that valid credentials are required
    socket->setHeader("WWW-Authenticate", QString("Basic realm=\"%1\"").arg(d->realm).toUtf8());
    socket->writeError(Socket::Unauthorized);
    return false;
}
