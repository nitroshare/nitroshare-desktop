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

#include <QList>
#include <QObject>
#include <QTest>

#include <qhttpengine/parser.h>
#include <qhttpengine/socket.h>
#include <qhttpengine/ibytearray.h>

typedef QList<QByteArray> QByteArrayList;

Q_DECLARE_METATYPE(QHttpEngine::Socket::Method)
Q_DECLARE_METATYPE(QHttpEngine::Socket::QueryStringMap)
Q_DECLARE_METATYPE(QHttpEngine::Socket::HeaderMap)

const QHttpEngine::IByteArray Key1 = "a";
const QByteArray Value1 = "b";
const QByteArray Line1 = Key1 + ": " + Value1;

const QHttpEngine::IByteArray Key2 = "c";
const QByteArray Value2 = "d";
const QByteArray Line2 = Key2 + ": " + Value2;

class TestParser : public QObject
{
    Q_OBJECT

public:

    TestParser();

private Q_SLOTS:

    void testSplit_data();
    void testSplit();

    void testParsePath_data();
    void testParsePath();

    void testParseHeaderList_data();
    void testParseHeaderList();

    void testParseHeaders_data();
    void testParseHeaders();

    void testParseRequestHeaders_data();
    void testParseRequestHeaders();

    void testParseResponseHeaders_data();
    void testParseResponseHeaders();

private:

    QHttpEngine::Socket::HeaderMap headers;
};

TestParser::TestParser()
{
    headers.insert(Key1, Value1);
    headers.insert(Key2, Value2);
}

void TestParser::testSplit_data()
{
    QTest::addColumn<QByteArray>("data");
    QTest::addColumn<QByteArray>("delim");
    QTest::addColumn<int>("maxSplit");
    QTest::addColumn<QByteArrayList>("parts");

    QTest::newRow("empty string")
            << QByteArray()
            << QByteArray(",")
            << 0
            << (QByteArrayList() << "");

    QTest::newRow("no delimiter")
            << QByteArray("a")
            << QByteArray(",")
            << 0
            << (QByteArrayList() << "a");

    QTest::newRow("delimiter")
            << QByteArray("a::b::c")
            << QByteArray("::")
            << 0
            << (QByteArrayList() << "a" << "b" << "c");

    QTest::newRow("empty parts")
            << QByteArray("a,,")
            << QByteArray(",")
            << 0
            << (QByteArrayList() << "a" << "" << "");

    QTest::newRow("maxSplit")
            << QByteArray("a,a,a")
            << QByteArray(",")
            << 1
            << (QByteArrayList() << "a" << "a,a");
}

void TestParser::testSplit()
{
    QFETCH(QByteArray, data);
    QFETCH(QByteArray, delim);
    QFETCH(int, maxSplit);
    QFETCH(QByteArrayList, parts);

    QByteArrayList outParts;
    QHttpEngine::Parser::split(data, delim, maxSplit, outParts);

    QCOMPARE(outParts, parts);
}

void TestParser::testParsePath_data()
{
    QTest::addColumn<QByteArray>("rawPath");
    QTest::addColumn<QString>("path");
    QTest::addColumn<QHttpEngine::Socket::QueryStringMap>("map");

    QTest::newRow("no query string")
            << QByteArray("/path")
            << QString("/path")
            << QHttpEngine::Socket::QueryStringMap();

    QTest::newRow("single parameter")
            << QByteArray("/path?a=b")
            << QString("/path")
            << QHttpEngine::Socket::QueryStringMap{{"a", "b"}};
}

void TestParser::testParsePath()
{
    QFETCH(QByteArray, rawPath);
    QFETCH(QString, path);
    QFETCH(QHttpEngine::Socket::QueryStringMap, map);

    QString outPath;
    QHttpEngine::Socket::QueryStringMap outMap;

    QVERIFY(QHttpEngine::Parser::parsePath(rawPath, outPath, outMap));

    QCOMPARE(path, outPath);
    QCOMPARE(map, outMap);
}

void TestParser::testParseHeaderList_data()
{
    QTest::addColumn<bool>("success");
    QTest::addColumn<QByteArrayList>("lines");
    QTest::addColumn<QHttpEngine::Socket::HeaderMap>("headers");

    QTest::newRow("empty line")
            << false
            << (QByteArrayList() << "");

    QTest::newRow("multiple lines")
            << true
            << (QByteArrayList() << Line1 << Line2)
            << headers;
}

void TestParser::testParseHeaderList()
{
    QFETCH(bool, success);
    QFETCH(QByteArrayList, lines);

    QHttpEngine::Socket::HeaderMap outHeaders;
    QCOMPARE(QHttpEngine::Parser::parseHeaderList(lines, outHeaders), success);

    if (success) {
        QFETCH(QHttpEngine::Socket::HeaderMap, headers);
        QCOMPARE(outHeaders, headers);
    }
}

void TestParser::testParseHeaders_data()
{
    QTest::addColumn<bool>("success");
    QTest::addColumn<QByteArray>("data");
    QTest::addColumn<QByteArrayList>("parts");

    QTest::newRow("empty headers")
            << false
            << QByteArray("");

    QTest::newRow("simple GET request")
            << true
            << QByteArray("GET / HTTP/1.0")
            << (QByteArrayList() << "GET" << "/" << "HTTP/1.0");
}

void TestParser::testParseHeaders()
{
    QFETCH(bool, success);
    QFETCH(QByteArray, data);

    QByteArrayList outParts;
    QHttpEngine::Socket::HeaderMap outHeaders;

    QCOMPARE(QHttpEngine::Parser::parseHeaders(data, outParts, outHeaders), success);

    if (success) {
        QFETCH(QByteArrayList, parts);
        QCOMPARE(outParts, parts);
    }
}

void TestParser::testParseRequestHeaders_data()
{
    QTest::addColumn<bool>("success");
    QTest::addColumn<QByteArray>("data");
    QTest::addColumn<QHttpEngine::Socket::Method>("method");
    QTest::addColumn<QByteArray>("path");

    QTest::newRow("bad HTTP version")
            << false
            << QByteArray("GET / HTTP/0.9");

    QTest::newRow("GET request")
            << true
            << QByteArray("GET / HTTP/1.0")
            << QHttpEngine::Socket::GET
            << QByteArray("/");
}

void TestParser::testParseRequestHeaders()
{
    QFETCH(bool, success);
    QFETCH(QByteArray, data);

    QHttpEngine::Socket::Method outMethod;
    QByteArray outPath;
    QHttpEngine::Socket::HeaderMap outHeaders;

    QCOMPARE(QHttpEngine::Parser::parseRequestHeaders(data, outMethod, outPath, outHeaders), success);

    if (success) {
        QFETCH(QHttpEngine::Socket::Method, method);
        QFETCH(QByteArray, path);

        QCOMPARE(method, outMethod);
        QCOMPARE(path, outPath);
    }
}

void TestParser::testParseResponseHeaders_data()
{
    QTest::addColumn<bool>("success");
    QTest::addColumn<QByteArray>("data");
    QTest::addColumn<int>("statusCode");
    QTest::addColumn<QByteArray>("statusReason");

    QTest::newRow("invalid status code")
            << false
            << QByteArray("HTTP/1.0 600 BAD RESPONSE");

    QTest::newRow("404 response")
            << true
            << QByteArray("HTTP/1.0 404 NOT FOUND")
            << 404
            << QByteArray("NOT FOUND");
}

void TestParser::testParseResponseHeaders()
{
    QFETCH(bool, success);
    QFETCH(QByteArray, data);

    int outStatusCode;
    QByteArray outStatusReason;
    QHttpEngine::Socket::HeaderMap outHeaders;

    QCOMPARE(QHttpEngine::Parser::parseResponseHeaders(data, outStatusCode, outStatusReason, outHeaders), success);

    if (success) {
        QFETCH(int, statusCode);
        QFETCH(QByteArray, statusReason);

        QCOMPARE(statusCode, outStatusCode);
        QCOMPARE(statusReason, outStatusReason);
    }
}

QTEST_MAIN(TestParser)
#include "TestParser.moc"
