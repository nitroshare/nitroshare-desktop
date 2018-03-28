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

#include <QHostAddress>
#include <QObject>
#include <QTest>

#include <qhttpengine/server.h>
#include <qhttpengine/socket.h>
#include <qhttpengine/qobjecthandler.h>
#include <qhttpengine/proxyhandler.h>

#include "common/qsimplehttpclient.h"
#include "common/qsocketpair.h"

const QString Path = "test";
const QByteArray Data = "test";

class TestProxyHandler : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void testDataPassthrough();
};

void TestProxyHandler::testDataPassthrough()
{
    // Create the upstream handler (simple echo)
    QHttpEngine::QObjectHandler upstreamHandler;
    upstreamHandler.registerMethod(Path, [](QHttpEngine::Socket *socket) {
        socket->write(socket->readAll());
        socket->close();
    }, true);

    // Create the upstream server and begin listening
    QHttpEngine::Server upstreamServer(&upstreamHandler);
    QVERIFY(upstreamServer.listen(QHostAddress::LocalHost));

    // Create the proxy handler
    QHttpEngine::ProxyHandler handler(upstreamServer.serverAddress(), upstreamServer.serverPort());

    QSocketPair pair;
    QTRY_VERIFY(pair.isConnected());

    QSimpleHttpClient client(pair.client());
    QHttpEngine::Socket *socket = new QHttpEngine::Socket(pair.server(), &pair);

    // Send the headers and wait for them to be parsed
    QHttpEngine::Socket::HeaderMap headers{
        {"Content-Length", QByteArray::number(Data.length())}
    };
    client.sendHeaders("POST", QString("/%1").arg(Path).toUtf8(), headers);
    QTRY_VERIFY(socket->isHeadersParsed());

    // Route the request (triggering the upstream connection)
    handler.route(socket, Path);

    // Send the data and wait for it to return
    client.sendData(Data);
    QTRY_COMPARE(client.data(), Data);
}

QTEST_MAIN(TestProxyHandler)
#include "TestProxyHandler.moc"
