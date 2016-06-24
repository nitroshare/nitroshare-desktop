/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Nathan Osman
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
 **/

#include <QFileInfo>
#include <QStringList>

#include "apihandler.h"
#include "config.h"

ApiHandler::ApiHandler(const QString &token)
    : mToken(token)
{
}

QVariantMap ApiHandler::version(const QVariantMap &params)
{
    return QVariantMap({
        { "version", PROJECT_VERSION }
    });
}

QVariantMap ApiHandler::sendItems(const QVariantMap &params)
{
    QStringList items = params.value("items").toStringList();
    Bundle *bundle = new Bundle();

    // Add the items to the bundle
    foreach (QString item, items) {
        QFileInfo info(item);
        if (info.isDir()) {
            bundle->addDirectory(item);
        } else if (info.isFile()) {
            bundle->addFile(item);
        }
    }

    // Transfer the bundle
    emit bundleCreated(bundle);

    return QVariantMap();
}

void ApiHandler::process(QHttpSocket *socket, const QString &path)
{
    // Ensure that the correct authentication token was provided
    if (socket->headers().value("X-Auth-Token") != mToken) {
        socket->writeError(QHttpSocket::Forbidden);
        return;
    }

    // If authenticated, have QObjectHandler process the request
    QObjectHandler::process(socket, path);
}
