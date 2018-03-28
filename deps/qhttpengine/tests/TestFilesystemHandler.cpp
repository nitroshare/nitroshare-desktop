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

#include <QDir>
#include <QFile>
#include <QObject>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QTest>

#include <qhttpengine/socket.h>
#include <qhttpengine/filesystemhandler.h>

#include "common/qsimplehttpclient.h"
#include "common/qsocketpair.h"

const QByteArray Data = "test";

class TestFilesystemHandler : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void initTestCase();

    void testRangeRequests_data();
    void testRangeRequests();

    void testRequests_data();
    void testRequests();

private:

    bool createFile(const QString &path);
    bool createDirectory(const QString &path);

    QTemporaryDir dir;
};

void TestFilesystemHandler::initTestCase()
{
    QVERIFY(createFile("outside"));
    QVERIFY(createDirectory("root"));
    QVERIFY(createFile("root/inside"));
}

void TestFilesystemHandler::testRequests_data()
{
    QTest::addColumn<QString>("path");
    QTest::addColumn<int>("statusCode");
    QTest::addColumn<QByteArray>("data");

    QTest::newRow("nonexistent resource")
            << "nonexistent"
            << static_cast<int>(QHttpEngine::Socket::NotFound)
            << QByteArray();

    QTest::newRow("outside document root")
            << "../outside"
            << static_cast<int>(QHttpEngine::Socket::NotFound)
            << QByteArray();

    QTest::newRow("inside document root")
            << "inside"
            << static_cast<int>(QHttpEngine::Socket::OK)
            << Data;

    QTest::newRow("directory listing")
            << ""
            << static_cast<int>(QHttpEngine::Socket::OK)
            << QByteArray();
}

void TestFilesystemHandler::testRequests()
{
    QFETCH(QString, path);
    QFETCH(int, statusCode);
    QFETCH(QByteArray, data);

    QHttpEngine::FilesystemHandler handler(QDir(dir.path()).absoluteFilePath("root"));

    QSocketPair pair;
    QTRY_VERIFY(pair.isConnected());

    QSimpleHttpClient client(pair.client());
    QHttpEngine::Socket *socket = new QHttpEngine::Socket(pair.server(), &pair);

    handler.route(socket, path);

    QTRY_COMPARE(client.statusCode(), statusCode);

    if (!data.isNull()) {
        QTRY_COMPARE(client.data(), data);
    }
}

void TestFilesystemHandler::testRangeRequests_data()
{
    QTest::addColumn<QString>("path");
    QTest::addColumn<QString>("range");
    QTest::addColumn<int>("statusCode");
    QTest::addColumn<QString>("contentRange");
    QTest::addColumn<QByteArray>("data");

    QTest::newRow("full file")
            << "inside" << ""
            << static_cast<int>(QHttpEngine::Socket::OK)
            << ""
            << Data;

    QTest::newRow("range 0-2")
            << "inside" << "0-2"
            << static_cast<int>(QHttpEngine::Socket::PartialContent)
            << "bytes 0-2/4"
            << Data.mid(0, 3);

    QTest::newRow("range 1-2")
            << "inside" << "1-2"
            << static_cast<int>(QHttpEngine::Socket::PartialContent)
            << "bytes 1-2/4"
            << Data.mid(1, 2);

    QTest::newRow("skip first 1 byte")
            << "inside" << "1-"
            << static_cast<int>(QHttpEngine::Socket::PartialContent)
            << "bytes 1-3/4"
            << Data.mid(1);

    QTest::newRow("last 2 bytes")
            << "inside" << "-2"
            << static_cast<int>(QHttpEngine::Socket::PartialContent)
            << "bytes 2-3/4"
            << Data.mid(2);

    QTest::newRow("bad range request")
            << "inside" << "abcd"
            << static_cast<int>(QHttpEngine::Socket::OK)
            << ""
            << Data;
}

void TestFilesystemHandler::testRangeRequests()
{
    QFETCH(QString, path);
    QFETCH(QString, range);
    QFETCH(int, statusCode);
    QFETCH(QString, contentRange);
    QFETCH(QByteArray, data);

    QHttpEngine::FilesystemHandler handler(QDir(dir.path()).absoluteFilePath("root"));

    QSocketPair pair;
    QTRY_VERIFY(pair.isConnected());

    QSimpleHttpClient client(pair.client());
    QHttpEngine::Socket *socket = new QHttpEngine::Socket(pair.server(), &pair);

    if (!range.isEmpty()) {
        QHttpEngine::Socket::HeaderMap inHeaders;
        inHeaders.insert("Range", QByteArray("bytes=") + range.toUtf8());
        client.sendHeaders("GET", path.toUtf8(), inHeaders);
        QTRY_VERIFY(socket->isHeadersParsed());
    }

    handler.route(socket, path);

    QTRY_COMPARE(client.statusCode(), statusCode);

    if (!data.isNull()) {
        QTRY_COMPARE(client.data(), data);
        QCOMPARE(client.headers().value("Content-Length").toInt(), data.length());
        QCOMPARE(client.headers().value("Content-Range"), contentRange.toLatin1());
    }
}

bool TestFilesystemHandler::createFile(const QString &path)
{
    QFile file(QDir(dir.path()).absoluteFilePath(path));
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    return file.write(Data) == Data.length();
}

bool TestFilesystemHandler::createDirectory(const QString &path)
{
    return QDir(dir.path()).mkpath(path);
}

QTEST_MAIN(TestFilesystemHandler)
#include "TestFilesystemHandler.moc"
