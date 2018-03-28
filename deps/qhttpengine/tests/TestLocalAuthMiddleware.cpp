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

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QScopedPointer>
#include <QTest>
#include <QVariantMap>

#include <qhttpengine/socket.h>
#include <qhttpengine/localauthmiddleware.h>

#include "common/qsimplehttpclient.h"
#include "common/qsocketpair.h"

const QByteArray HeaderName = "X-Test";
const QByteArray CustomName = "Name";
const QByteArray CustomData = "Data";

class TestLocalAuthMiddleware : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void testAuth();
    void testRemoval();
};

void TestLocalAuthMiddleware::testAuth()
{
    QSocketPair pair;
    QTRY_VERIFY(pair.isConnected());

    QSimpleHttpClient client(pair.client());
    QHttpEngine::Socket socket(pair.server(), &pair);

    QHttpEngine::LocalAuthMiddleware localAuth;
    localAuth.setData(QVariantMap{
        {CustomName, CustomData}
    });
    localAuth.setHeaderName(HeaderName);
    QVERIFY(localAuth.exists());

    QFile file(localAuth.filename());
    QVERIFY(file.open(QIODevice::ReadOnly));

    QVariantMap data = QJsonDocument::fromJson(file.readAll()).object().toVariantMap();
    QVERIFY(data.contains("token"));
    QCOMPARE(data.value(CustomName).toByteArray(), CustomData);

    client.sendHeaders("GET", "/", QHttpEngine::Socket::HeaderMap{
        {HeaderName, data.value("token").toByteArray()}
    });
    QTRY_VERIFY(socket.isHeadersParsed());

    QVERIFY(localAuth.process(&socket));
}

void TestLocalAuthMiddleware::testRemoval()
{
    QScopedPointer<QHttpEngine::LocalAuthMiddleware> localAuth(
                new QHttpEngine::LocalAuthMiddleware);
    QString filename = localAuth->filename();

    QVERIFY(QFile::exists(filename));
    delete localAuth.take();
    QVERIFY(!QFile::exists(filename));
}

QTEST_MAIN(TestLocalAuthMiddleware)
#include "TestLocalAuthMiddleware.moc"
