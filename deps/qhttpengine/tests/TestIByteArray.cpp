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

#include <QObject>
#include <QTest>

#include <qhttpengine/ibytearray.h>

const char *Value1 = "test";
const char *Value2 = "TEST";

// Helpful macros to cut down on the amount of duplicated code
#define TEST_OPERATOR(tn,t,on,o,v) void test##tn##on() \
    { \
        QCOMPARE(QHttpEngine::IByteArray(Value1) o static_cast<t>(Value2), v); \
        QCOMPARE(static_cast<t>(Value1) o QHttpEngine::IByteArray(Value1), v); \
    }
#define TEST_TYPE(tn,t) \
    TEST_OPERATOR(tn, t, Equals, ==, true) \
    TEST_OPERATOR(tn, t, NotEquals, !=, false) \
    TEST_OPERATOR(tn, t, Less, <, false) \
    TEST_OPERATOR(tn, t, Greater, >, false) \
    TEST_OPERATOR(tn, t, LessEqual, <=, true) \
    TEST_OPERATOR(tn, t, GreaterEqual, >=, true)

class TestIByteArray : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    TEST_TYPE(ConstChar, const char *)
    TEST_TYPE(QByteArray, QByteArray)
    TEST_TYPE(IByteArray, QHttpEngine::IByteArray)
    TEST_TYPE(QString, QString)

    void testContains();
};

void TestIByteArray::testContains()
{
    QHttpEngine::IByteArray v(Value1);

    QVERIFY(v.contains('t'));
    QVERIFY(v.contains(Value2));
    QVERIFY(v.contains(QByteArray(Value2)));
}

QTEST_MAIN(TestIByteArray)
#include "TestIByteArray.moc"
