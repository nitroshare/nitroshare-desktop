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

#include <QJsonDocument>
#include <QJsonObject>
#include <QUuid>

#include <qhttpengine/localauthmiddleware.h>
#include <qhttpengine/socket.h>

#include "localauthmiddleware_p.h"

using namespace QHttpEngine;

LocalAuthMiddlewarePrivate::LocalAuthMiddlewarePrivate(QObject *parent)
    : QObject(parent),
      tokenHeader("X-Auth-Token"),
      token(QUuid::createUuid().toString())
{
    updateFile();
}

LocalAuthMiddlewarePrivate::~LocalAuthMiddlewarePrivate()
{
    file.remove();
}

void LocalAuthMiddlewarePrivate::updateFile()
{
    if (file.open()) {
        file.write(QJsonDocument(QJsonObject::fromVariantMap(data)).toJson());
        file.close();
    }
}

LocalAuthMiddleware::LocalAuthMiddleware(QObject *parent)
    : Middleware(parent),
      d(new LocalAuthMiddlewarePrivate(this))
{
}

bool LocalAuthMiddleware::exists() const
{
    return d->file.exists();
}

QString LocalAuthMiddleware::filename() const
{
    return d->file.fileName();
}

void LocalAuthMiddleware::setData(const QVariantMap &data)
{
    d->data = data;
    d->data.insert("token", d->token);
    d->updateFile();
}

void LocalAuthMiddleware::setHeaderName(const QByteArray &name)
{
    d->tokenHeader = name;
}

bool LocalAuthMiddleware::process(Socket *socket)
{
    if (socket->headers().value(d->tokenHeader) != d->token) {
        socket->writeError(Socket::Forbidden);
        return false;
    }

    return true;
}
