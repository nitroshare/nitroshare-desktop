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

#include <qhttpengine/basicauthmiddleware.h>
#include <qhttpengine/handler.h>
#include <qhttpengine/socket.h>

#include "common/qsimplehttpclient.h"
#include "common/qsocketpair.h"

const QString Username = "username";
const QString Password = "password";

class TestBasicAuthMiddleware : public QObject
{
    Q_OBJECT

public:

    TestBasicAuthMiddleware() : auth("Test") {}

private Q_SLOTS:

    void initTestCase();

    void testProcess_data();
    void testProcess();

private:

    QHttpEngine::BasicAuthMiddleware auth;
};

void TestBasicAuthMiddleware::initTestCase()
{
    auth.add(Username, Password);
}

void TestBasicAuthMiddleware::testProcess_data()
{
    QTest::addColumn<bool>("header");
    QTest::addColumn<QString>("username");
    QTest::addColumn<QString>("password");
    QTest::addColumn<int>("status");

    QTest::newRow("no header")
            << false
            << QString()
            << QString()
            << static_cast<int>(QHttpEngine::Socket::Unauthorized);

    QTest::newRow("invalid credentials")
            << true
            << Username
            << QString()
            << static_cast<int>(QHttpEngine::Socket::Unauthorized);

    QTest::newRow("valid credentials")
            << true
            << Username
            << Password
            << static_cast<int>(QHttpEngine::Socket::NotFound);
}

void TestBasicAuthMiddleware::testProcess()
{
    QFETCH(bool, header);
    QFETCH(QString, username);
    QFETCH(QString, password);
    QFETCH(int, status);

    QSocketPair pair;
    QTRY_VERIFY(pair.isConnected());

    QSimpleHttpClient client(pair.client());
    QHttpEngine::Socket *socket = new QHttpEngine::Socket(pair.server(), &pair);

    QHttpEngine::Socket::HeaderMap headers;

    if (header) {
        headers.insert(
            "Authorization",
            "Basic " + QString("%1:%2").arg(username).arg(password).toUtf8().toBase64()
        );
    }

    client.sendHeaders("GET", "/", headers);
    QTRY_VERIFY(socket->isHeadersParsed());

    QHttpEngine::Handler handler;
    handler.addMiddleware(&auth);
    handler.route(socket, "/");

    QTRY_COMPARE(client.statusCode(), status);
}

QTEST_MAIN(TestBasicAuthMiddleware)
#include "TestBasicAuthMiddleware.moc"
