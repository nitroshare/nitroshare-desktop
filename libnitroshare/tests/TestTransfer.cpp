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
#include <QTest>

#include <nitroshare/application.h>
#include <nitroshare/bundle.h>
#include <nitroshare/handlerregistry.h>
#include <nitroshare/transfer.h>
#include <nitroshare/transportserverregistry.h>

#include "mock/mockapplication.h"
#include "mock/mockdevice.h"
#include "mock/mockhandler.h"
#include "mock/mockitem.h"
#include "mock/mocktransport.h"
#include "mock/mocktransportserver.h"

class TestTransfer : public QObject
{
    Q_OBJECT

private slots:

    void initTestCase();

    void testSending();
    void testReceiving();

private:

    MockApplication mApplication;
    MockHandler mHandler;
    MockTransportServer mTransportServer;
};

void TestTransfer::initTestCase()
{
    mApplication.application()->handlerRegistry()->add(&mHandler);
    mApplication.application()->transportServerRegistry()->add(&mTransportServer);
}

void TestTransfer::testSending()
{
    MockDevice device;
    Bundle bundle;
    bundle.add(new MockItem);
    Transfer *transfer = new Transfer(mApplication.application(), &device, &bundle);

    // The device saves a copy of the transport so that it can be retrieved
    MockTransport *transport = device.transport();

    QCOMPARE(transfer->direction(), Transfer::Send);
    QCOMPARE(transfer->state(), Transfer::Connecting);

    // Have the transport complete the connection
    transport->emitConnected();

    QCOMPARE(transfer->state(), Transfer::InProgress);

    // The first two packets should be JSON data (the transfer & item headers)
    // and the third will be the payload
    QTRY_COMPARE(transport->packets().count(), 3);

    const MockTransport::PacketList &packets = transport->packets();

    // Ensure the transfer header was sent correctly
    QCOMPARE(packets.at(0).first, Packet::Json);
    QJsonObject transferHeader{
        { "name", MockApplication::DeviceName },
        { "size", QString::number(MockItem::Data.size()) },
        { "count", QString::number(1) }
    };
    QCOMPARE(QJsonDocument::fromJson(packets.at(0).second).object(), transferHeader);

    // Ensure the item header was sent correctly
    QCOMPARE(packets.at(1).first, Packet::Json);
    QJsonObject itemHeader{
        { "name", MockItem::Name },
        { "type", MockItem::Type },
        { "size", QString::number(MockItem::Data.size()) }
    };
    QCOMPARE(QJsonDocument::fromJson(packets.at(1).second).object(), itemHeader);

    // Ensure the item was sent correctly
    QCOMPARE(packets.at(2).first, Packet::Binary);
    QCOMPARE(packets.at(2).second, MockItem::Data);

    QCOMPARE(transfer->state(), Transfer::InProgress);

    // Send the success packet
    transport->sendData(Packet::Success);

    QCOMPARE(transfer->state(), Transfer::Succeeded);
    QVERIFY(transport->isClosed());
}

void TestTransfer::testReceiving()
{
    MockTransport transport;
    Transfer *transfer = new Transfer(mApplication.application(), &transport);

    QCOMPARE(transfer->direction(), Transfer::Receive);
    QCOMPARE(transfer->state(), Transfer::InProgress);

    // Send the transfer header to the transport
    QJsonObject transferHeader{
        { "name", MockDevice::Name },
        { "size", QString::number(MockItem::Data.size()) },
        { "count", QString::number(1) }
    };
    transport.sendData(Packet::Json, QJsonDocument(transferHeader).toJson());

    QCOMPARE(transfer->deviceName(), MockDevice::Name);

    // Send the item header to the transport followed by the data for the item
    QJsonObject itemHeader{
        { "name", MockItem::Name },
        { "type", MockItem::Type },
        { "size", QString::number(MockItem::Data.size()) }
    };
    transport.sendData(Packet::Json, QJsonDocument(itemHeader).toJson());
    transport.sendData(Packet::Binary, MockItem::Data);

    QTRY_COMPARE(transfer->state(), Transfer::Succeeded);

    // Ensure a success packet was sent and the transport closed
    QCOMPARE(transport.packets().count(), 1);
    QCOMPARE(transport.packets().at(0).first, Packet::Success);
    QVERIFY(transport.isClosed());
}

QTEST_MAIN(TestTransfer)
#include "TestTransfer.moc"
