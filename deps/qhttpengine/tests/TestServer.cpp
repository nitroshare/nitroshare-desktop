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

#include <QTcpSocket>
#include <QTest>

#if !defined(QT_NO_SSL)
#  include <QFile>
#  include <QSslCertificate>
#  include <QSslConfiguration>
#  include <QSslKey>
#  include <QSslSocket>
#endif

#include <qhttpengine/server.h>
#include <qhttpengine/handler.h>

#include "common/qsimplehttpclient.h"

class TestHandler : public QHttpEngine::Handler
{
    Q_OBJECT

public:

    virtual void process(QHttpEngine::Socket *socket, const QString &path) {
        mPath = path;
        socket->deleteLater();
    }

    QString mPath;
};

class TestServer : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void testServer();

#if !defined(QT_NO_SSL)
    void testSsl();
#endif
};

void TestServer::testServer()
{
    TestHandler handler;
    QHttpEngine::Server server(&handler);

    QVERIFY(server.listen(QHostAddress::LocalHost));

    QTcpSocket socket;
    socket.connectToHost(server.serverAddress(), server.serverPort());
    QTRY_COMPARE(socket.state(), QAbstractSocket::ConnectedState);

    QSimpleHttpClient client(&socket);
    client.sendHeaders("GET", "/test");

    QTRY_COMPARE(handler.mPath, QString("test"));
}

#if !defined(QT_NO_SSL)
void TestServer::testSsl()
{
    QFile keyFile(":/key.pem");
    QVERIFY(keyFile.open(QIODevice::ReadOnly));

    QSslKey key(&keyFile, QSsl::Rsa);
    QList<QSslCertificate> certs = QSslCertificate::fromPath(":/cert.pem");

    QSslConfiguration config;
    config.setPrivateKey(key);
    config.setLocalCertificateChain(certs);

    QHttpEngine::Server server;
    server.setSslConfiguration(config);

    QVERIFY(server.listen(QHostAddress::LocalHost));

    QSslSocket socket;
    socket.setCaCertificates(certs);
    socket.connectToHost(server.serverAddress(), server.serverPort());
    socket.setPeerVerifyName("localhost");

    QTRY_COMPARE(socket.state(), QAbstractSocket::ConnectedState);

    socket.startClientEncryption();
    QTRY_VERIFY(socket.isEncrypted());
}
#endif

QTEST_MAIN(TestServer)
#include "TestServer.moc"
