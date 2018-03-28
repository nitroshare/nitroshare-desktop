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

#include <QTest>

#include <qhttpengine/handler.h>
#include <qhttpengine/middleware.h>
#include <qhttpengine/socket.h>

#include "common/qsimplehttpclient.h"
#include "common/qsocketpair.h"

class DummyMiddleware : public QHttpEngine::Middleware
{
    Q_OBJECT

public:

    virtual bool process(QHttpEngine::Socket *socket)
    {
        socket->writeError(QHttpEngine::Socket::Forbidden);
        return false;
    }
};

class TestMiddleware : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void testProcess();
};

void TestMiddleware::testProcess()
{
    QSocketPair pair;
    QTRY_VERIFY(pair.isConnected());

    QSimpleHttpClient client(pair.client());
    QHttpEngine::Socket *socket = new QHttpEngine::Socket(pair.server(), &pair);

    client.sendHeaders("GET", "/");
    QTRY_VERIFY(socket->isHeadersParsed());

    DummyMiddleware middleware;
    QHttpEngine::Handler handler;
    handler.addMiddleware(&middleware);
    handler.route(socket, "/");

    QTRY_COMPARE(client.statusCode(), static_cast<int>(QHttpEngine::Socket::Forbidden));
}

QTEST_MAIN(TestMiddleware)
#include "TestMiddleware.moc"
