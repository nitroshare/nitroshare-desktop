/*
 * The MIT License (MIT)
 *
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
#include <QList>
#include <QTest>

#include <nitroshare/application.h>
#include <nitroshare/handlerregistry.h>
#include <nitroshare/transfer.h>

#include "mock/mockhandler.h"
#include "mock/mocktransport.h"

const QString MockDeviceName = "Test";
const QString MockItemName = "test.txt";
const QByteArray MockItemData = "test";

const QJsonObject MockTransferHeader{
    { "name", MockDeviceName },
    { "size", QString::number(MockItemData.size()) },
    { "count", QString::number(1) }
};

const QJsonObject MockItemHeader{
    { "name", MockItemName },
    { "type", MockHandler::Name },
    { "size", QString::number(MockItemData.size()) }
};

class TestTransfer : public QObject
{
    Q_OBJECT

private slots:

    void testReceiving();
};

void TestTransfer::testReceiving()
{
    Application application;
    MockTransport transport;
    Transfer *transfer = new Transfer(&application, &transport);

    MockHandler handler;
    application.handlerRegistry()->add(&handler);

    QCOMPARE(transfer->state(), Transfer::InProgress);

    // Send the transfer header to the transport
    transport.writeData(Packet::Json, QJsonDocument(MockTransferHeader).toJson());

    QCOMPARE(transfer->deviceName(), MockDeviceName);

    // Send the item header to the transport followed by the data for the item
    transport.writeData(Packet::Json, QJsonDocument(MockItemHeader).toJson());
    transport.writeData(Packet::Binary, MockItemData);

    QTRY_COMPARE(transfer->state(), Transfer::Succeeded);
}

QTEST_MAIN(TestTransfer)
#include "TestTransfer.moc"
