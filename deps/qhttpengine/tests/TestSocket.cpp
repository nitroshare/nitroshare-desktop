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
#include <QObject>
#include <QSignalSpy>
#include <QTest>

#include <qhttpengine/socket.h>
#include <qhttpengine/parser.h>

#include "common/qsimplehttpclient.h"
#include "common/qsocketpair.h"

Q_DECLARE_METATYPE(QHttpEngine::Socket::QueryStringMap)

// Utility macro (avoids duplication) that creates a pair of connected
// sockets, a QSimpleHttpClient for the client and a QHttpSocket for the
// server
#define CREATE_SOCKET_PAIR() \
    QSocketPair pair; \
    QTRY_VERIFY(pair.isConnected()); \
    QSimpleHttpClient client(pair.client()); \
    QHttpEngine::Socket *server = new QHttpEngine::Socket(pair.server(), &pair);

const QByteArray Method = "POST";
const QByteArray Path = "/test";
const int StatusCode = 404;
const QByteArray StatusReason = "NOT FOUND";
const QByteArray Data = "test";

class TestSocket : public QObject
{
    Q_OBJECT

public:

    TestSocket();

private Q_SLOTS:

    void testProperties();
    void testData();
    void testRedirect();
    void testSignals();
    void testJson();

private:

    QHttpEngine::Socket::HeaderMap headers;
};

TestSocket::TestSocket()
{
    headers.insert("Content-Type", "text/plain");
    headers.insert("Content-Length", QByteArray::number(Data.length()));
}

void TestSocket::testProperties()
{
    CREATE_SOCKET_PAIR();

    client.sendHeaders(Method, Path, headers);

    QTRY_VERIFY(server->isHeadersParsed());
    QCOMPARE(server->method(), QHttpEngine::Socket::POST);
    QCOMPARE(server->rawPath(), Path);
    QCOMPARE(server->headers(), headers);

    server->setStatusCode(StatusCode, StatusReason);
    server->setHeaders(headers);
    server->writeHeaders();

    QTRY_COMPARE(client.statusCode(), StatusCode);
    QCOMPARE(client.statusReason(), StatusReason);
    QCOMPARE(client.headers(), headers);
}

void TestSocket::testData()
{
    CREATE_SOCKET_PAIR();

    client.sendHeaders(Method, Path, headers);
    client.sendData(Data);

    QTRY_COMPARE(server->contentLength(), Data.length());
    QTRY_COMPARE(server->bytesAvailable(), Data.length());
    QCOMPARE(server->readAll(), Data);

    server->writeHeaders();
    server->write(Data);

    QTRY_COMPARE(client.data(), Data);
}

void TestSocket::testRedirect()
{
    CREATE_SOCKET_PAIR();

    QSignalSpy disconnectedSpy(pair.client(), SIGNAL(disconnected()));

    server->writeRedirect(Path, true);

    QTRY_COMPARE(client.statusCode(), static_cast<int>(QHttpEngine::Socket::MovedPermanently));
    QCOMPARE(client.headers().value("Location"), Path);
    QTRY_COMPARE(disconnectedSpy.count(), 1);
}

void TestSocket::testSignals()
{
    CREATE_SOCKET_PAIR();

    QSignalSpy headersParsedSpy(server, SIGNAL(headersParsed()));
    QSignalSpy readyReadSpy(server, SIGNAL(readyRead()));
    QSignalSpy bytesWrittenSpy(server, SIGNAL(bytesWritten(qint64)));
    QSignalSpy aboutToCloseSpy(server, SIGNAL(aboutToClose()));
    QSignalSpy readChannelFinishedSpy(server, SIGNAL(readChannelFinished()));

    client.sendHeaders(Method, Path, headers);

    QTRY_COMPARE(headersParsedSpy.count(), 1);
    QCOMPARE(readyReadSpy.count(), 0);

    client.sendData(Data);

    QTRY_COMPARE(server->bytesAvailable(), Data.length());
    QVERIFY(readyReadSpy.count() > 0);

    server->writeHeaders();
    server->write(Data);

    QTRY_COMPARE(client.data().length(), Data.length());
    QVERIFY(bytesWrittenSpy.count() > 0);

    qint64 bytesWritten = 0;
    for (int i = 0; i < bytesWrittenSpy.count(); ++i) {
        bytesWritten += bytesWrittenSpy.at(i).at(0).toLongLong();
    }
    QCOMPARE(bytesWritten, Data.length());

    QTRY_COMPARE(aboutToCloseSpy.count(), 0);
    server->close();
    QTRY_COMPARE(aboutToCloseSpy.count(), 1);

    QCOMPARE(readChannelFinishedSpy.count(), 1);
}

void TestSocket::testJson()
{
    CREATE_SOCKET_PAIR();

    QJsonObject object{{"a", "b"}, {"c", 123}};
    QByteArray data = QJsonDocument(object).toJson();

    client.sendHeaders(Method, Path, QHttpEngine::Socket::HeaderMap{
        {"Content-Length", QByteArray::number(data.length())},
        {"Content-Type", "application/json"}
    });
    client.sendData(data);

    QTRY_VERIFY(server->isHeadersParsed());
    QTRY_VERIFY(server->bytesAvailable() >= server->contentLength());

    QJsonDocument document;
    QVERIFY(server->readJson(document));
    QCOMPARE(document.object(), object);
}

QTEST_MAIN(TestSocket)
#include "TestSocket.moc"
