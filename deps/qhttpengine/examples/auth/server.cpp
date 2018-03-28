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

#include <QCoreApplication>
#include <QHostAddress>
#include <QVariantMap>

#include <qhttpengine/handler.h>
#include <qhttpengine/localauthmiddleware.h>
#include <qhttpengine/qobjecthandler.h>
#include <qhttpengine/server.h>
#include <qhttpengine/socket.h>

int main(int argc, char * argv[])
{
    QCoreApplication a(argc, argv);

    QHttpEngine::QObjectHandler handler;
    handler.registerMethod("", [](QHttpEngine::Socket *socket) {
        socket->setStatusCode(QHttpEngine::Socket::OK);
        socket->writeHeaders();
        socket->close();
    });

    QHttpEngine::Server server(&handler);
    if (!server.listen(QHostAddress::LocalHost)) {
        qCritical("unable to bind to a local port");
        a.exit(1);
    }

    QHttpEngine::LocalAuthMiddleware middleware;
    middleware.setData(QVariantMap{
        {"port", server.serverPort()},
    });
    handler.addMiddleware(&middleware);

    return a.exec();
}
