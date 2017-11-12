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
#include <nitroshare/bundle.h>
#include <nitroshare/handlerregistry.h>
#include <nitroshare/transfer.h>
#include <nitroshare/transfermodel.h>

#include "mock/mockdevice.h"
#include "mock/mockhandler.h"
#include "mock/mockitem.h"
#include "mock/mocktransport.h"
#include "mock/mocktransportserver.h"

const QString MockItemName = "test.txt";
const QByteArray MockItemData = "test";

const QJsonObject MockTransferHeader{
    { "name", MockDevice::Name },
    { "size", QString::number(MockItemData.size()) },
    { "count", QString::number(1) }
};

const QJsonObject MockItemHeader{
    { "name", MockItemName },
    { "type", MockItem::Type },
    { "size", QString::number(MockItemData.size()) }
};

class TestTransfer : public QObject
{
    Q_OBJECT

private slots:

    void initTestCase();

    void testSending();
    void testReceiving();

private:

    Application mApplication;
    MockHandler mHandler;
    MockTransportServer mTransportServer;
};

void TestTransfer::initTestCase()
{
    mApplication.handlerRegistry()->add(&mHandler);
    mApplication.transferModel()->addTransportServer(&mTransportServer);
}

void TestTransfer::testSending()
{
    MockTransport transport;
    Bundle bundle;
    bundle.add(new MockItem(MockItemName, MockItemData));
    Transfer *transfer = new Transfer(&mApplication, &transport, &bundle, MockDevice::Name);

    QCOMPARE(transfer->state(), Transfer::Connecting);

    // Have the transport complete the connection
    transport.emitConnected();

    QCOMPARE(transfer->state(), Transfer::InProgress);

    // The first two packets should be JSON data (the transfer & item headers)
    // and the third will be the payload
    QTRY_COMPARE(transport.packets().count(), 3);

    const MockTransport::PacketList &packets = transport.packets();

    // Ensure the transfer header was sent correctly
    QCOMPARE(packets.at(0).first, Packet::Json);
    QCOMPARE(QJsonDocument::fromJson(packets.at(0).second).object(), MockTransferHeader);

    // Ensure the item header was sent correctly
    QCOMPARE(packets.at(1).first, Packet::Json);
    QCOMPARE(QJsonDocument::fromJson(packets.at(1).second).object(), MockItemHeader);

    // Ensure the item was sent correctly
    QCOMPARE(packets.at(2).first, Packet::Binary);
    QCOMPARE(packets.at(2).second, MockItemData);

    QCOMPARE(transfer->state(), Transfer::InProgress);

    // Send the success packet
    transport.sendData(Packet::Success);

    QCOMPARE(transfer->state(), Transfer::Succeeded);
    QVERIFY(transport.isClosed());
}

void TestTransfer::testReceiving()
{
    MockTransport transport;
    Transfer *transfer = new Transfer(&mApplication, &transport);

    QCOMPARE(transfer->state(), Transfer::InProgress);

    // Send the transfer header to the transport
    transport.sendData(Packet::Json, QJsonDocument(MockTransferHeader).toJson());

    QCOMPARE(transfer->deviceName(), MockDevice::Name);

    // Send the item header to the transport followed by the data for the item
    transport.sendData(Packet::Json, QJsonDocument(MockItemHeader).toJson());
    transport.sendData(Packet::Binary, MockItemData);

    QTRY_COMPARE(transfer->state(), Transfer::Succeeded);

    // Ensure a success packet was sent and the transport closed
    QCOMPARE(transport.packets().count(), 1);
    QCOMPARE(transport.packets().at(0).first, Packet::Success);
    QVERIFY(transport.isClosed());
}

QTEST_MAIN(TestTransfer)
#include "TestTransfer.moc"
