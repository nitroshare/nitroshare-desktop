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

#include <QBuffer>
#include <QObject>
#include <QSignalSpy>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTest>

#include <qhttpengine/qiodevicecopier.h>

#include "common/qsocketpair.h"

const QByteArray SampleData = "1234567890123456789012345678901234567890";

class TestQIODeviceCopier : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void testRange_data();
    void testRange();

    void testQBuffer();
    void testQTcpSocket();
    void testStop();
};

void TestQIODeviceCopier::testQBuffer()
{
    QBuffer src;
    src.setData(SampleData);

    QByteArray destData;
    QBuffer dest(&destData);

    QHttpEngine::QIODeviceCopier copier(&src, &dest);
    copier.setBufferSize(2);

    QSignalSpy errorSpy(&copier, SIGNAL(error(QString)));
    QSignalSpy finishedSpy(&copier, SIGNAL(finished()));

    copier.start();

    QTRY_COMPARE(finishedSpy.count(), 1);
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(destData, SampleData);
}

void TestQIODeviceCopier::testQTcpSocket()
{
    QSocketPair pair;
    QTRY_VERIFY(pair.isConnected());

    QByteArray destData;
    QBuffer dest(&destData);

    QHttpEngine::QIODeviceCopier copier(pair.server(), &dest);
    copier.setBufferSize(2);

    QSignalSpy errorSpy(&copier, SIGNAL(error(QString)));
    QSignalSpy finishedSpy(&copier, SIGNAL(finished()));

    copier.start();

    pair.client()->write(SampleData);
    pair.client()->close();

    QTRY_COMPARE(finishedSpy.count(), 1);
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(destData, SampleData);
}

void TestQIODeviceCopier::testStop()
{
    QSocketPair pair;
    QTRY_VERIFY(pair.isConnected());

    QByteArray destData;
    QBuffer dest(&destData);

    QHttpEngine::QIODeviceCopier copier(pair.server(), &dest);

    copier.start();

    pair.client()->write(SampleData);
    QTRY_COMPARE(destData, SampleData);

    copier.stop();

    pair.client()->write(SampleData);
    QTRY_COMPARE(destData, SampleData);
}

void TestQIODeviceCopier::testRange_data()
{
    QTest::addColumn<int>("from");
    QTest::addColumn<int>("to");
    QTest::addColumn<int>("bufferSize");

    QTest::newRow("range: 1-21, bufSize: 8")
            << 1 << 21 << 8;

    QTest::newRow("range: 0-21, bufSize: 7")
            << 0 << 21 << 7;

    QTest::newRow("range: 10-, bufSize: 5")
            << 10 << -1 << 5;
}

void TestQIODeviceCopier::testRange()
{
    QFETCH(int, from);
    QFETCH(int, to);
    QFETCH(int, bufferSize);

    QBuffer src;
    src.setData(SampleData);

    QByteArray destData;
    QBuffer dest(&destData);

    QHttpEngine::QIODeviceCopier copier(&src, &dest);
    copier.setBufferSize(bufferSize);
    copier.setRange(from, to);

    QSignalSpy errorSpy(&copier, SIGNAL(error(QString)));
    QSignalSpy finishedSpy(&copier, SIGNAL(finished()));

    copier.start();

    QTRY_COMPARE(finishedSpy.count(), 1);
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(destData, SampleData.mid(from, to - from + 1));
}

QTEST_MAIN(TestQIODeviceCopier)
#include "TestQIODeviceCopier.moc"
