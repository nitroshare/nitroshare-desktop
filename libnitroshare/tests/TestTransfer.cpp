/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Nathan Osman
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
#include <QSignalSpy>
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

Q_DECLARE_METATYPE(Transfer::State)

const QString ErrorMessage = "test";

class TestTransfer : public QObject
{
    Q_OBJECT

private slots:

    void initTestCase();

    void testSending();
    void testReceiving();
    void testAbort();

private:

    MockApplication mApplication;
    MockHandler mHandler;
    MockTransportServer mTransportServer;
};

void TestTransfer::initTestCase()
{
    qRegisterMetaType<Transfer::State>("State");

    mApplication.application()->handlerRegistry()->add(&mHandler);
    mApplication.application()->transportServerRegistry()->add(&mTransportServer);
}

void TestTransfer::testSending()
{
    MockDevice device;
    Bundle *bundle = new Bundle;
    bundle->add(new MockItem);
    Transfer transfer(mApplication.application(), &device, bundle);

    // The device saves a copy of the transport so that it can be retrieved
    MockTransport *transport = device.transport();

    QCOMPARE(transfer.direction(), Transfer::Send);
    QCOMPARE(transfer.state(), Transfer::Connecting);

    QSignalSpy stateChangedSpy(&transfer, &Transfer::stateChanged);

    // Have the transport complete the connection
    transport->emitConnected();

    QCOMPARE(stateChangedSpy.count(), 1);
    QCOMPARE(stateChangedSpy.at(0).at(0), QVariant::fromValue(Transfer::InProgress));
    QCOMPARE(transfer.state(), Transfer::InProgress);

    QSignalSpy progressChangedSpy(&transfer, &Transfer::progressChanged);

    // The first two packets should be JSON data (the transfer & item headers)
    // and the third will be the payload
    QTRY_COMPARE(transport->packets().count(), 3);

    const MockTransport::PacketList &packets = transport->packets();

    // Ensure progress reached 100%
    QVERIFY(progressChangedSpy.count());
    QCOMPARE(progressChangedSpy.last().at(0), QVariant(100));
    QCOMPARE(transfer.progress(), 100);

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

    QCOMPARE(transfer.state(), Transfer::InProgress);

    // Send the success packet
    transport->sendData(Packet::Success);

    QCOMPARE(stateChangedSpy.count(), 2);
    QCOMPARE(stateChangedSpy.at(1).at(0), QVariant::fromValue(Transfer::Succeeded));
    QCOMPARE(transfer.state(), Transfer::Succeeded);

    QVERIFY(transport->isClosed());
}

void TestTransfer::testReceiving()
{
    MockTransport *transport = new MockTransport;
    Transfer transfer(mApplication.application(), transport);

    QCOMPARE(transfer.direction(), Transfer::Receive);
    QCOMPARE(transfer.state(), Transfer::InProgress);

    QSignalSpy deviceNameChangedSpy(&transfer, &Transfer::deviceNameChanged);

    // Send the transfer header to the transport
    QJsonObject transferHeader{
        { "name", MockDevice::Name },
        { "size", QString::number(MockItem::Data.size()) },
        { "count", QString::number(1) }
    };
    transport->sendData(Packet::Json, QJsonDocument(transferHeader).toJson());

    QCOMPARE(deviceNameChangedSpy.count(), 1);
    QCOMPARE(deviceNameChangedSpy.at(0).at(0), QVariant(MockDevice::Name));
    QCOMPARE(transfer.deviceName(), MockDevice::Name);

    // Send the item header to the transport
    QJsonObject itemHeader{
        { "name", MockItem::Name },
        { "type", MockItem::Type },
        { "size", QString::number(MockItem::Data.size()) }
    };
    transport->sendData(Packet::Json, QJsonDocument(itemHeader).toJson());

    QSignalSpy progressChangedSpy(&transfer, &Transfer::progressChanged);
    QSignalSpy stateChangedSpy(&transfer, &Transfer::stateChanged);

    // Send item data
    transport->sendData(Packet::Binary, MockItem::Data);

    // Ensure progress reached 100%
    QVERIFY(progressChangedSpy.count() > 0);
    QCOMPARE(progressChangedSpy.last().at(0), QVariant(100));
    QCOMPARE(transfer.progress(), 100);

    // Ensure the transfer succeeded
    QTRY_COMPARE(stateChangedSpy.count(), 1);
    QCOMPARE(stateChangedSpy.at(0).at(0), QVariant::fromValue(Transfer::Succeeded));
    QCOMPARE(transfer.state(), Transfer::Succeeded);

    // Ensure a success packet was sent and the transport closed
    QCOMPARE(transport->packets().count(), 1);
    QCOMPARE(transport->packets().at(0).first, Packet::Success);
    QVERIFY(transport->isClosed());
}

void TestTransfer::testAbort()
{
    MockTransport *transport = new MockTransport;
    Transfer transfer(mApplication.application(), transport);

    // Send a header, though it will be ignored
    QJsonObject transferHeader{
        { "name", MockDevice::Name },
        { "size", QString::number(1) },
        { "count", QString::number(1) }
    };
    transport->sendData(Packet::Json, QJsonDocument(transferHeader).toJson());

    // Watch for the error signal
    QSignalSpy stateChangedSpy(&transfer, &Transfer::stateChanged);
    QSignalSpy errorChangedSpy(&transfer, &Transfer::errorChanged);

    // Abort the transfer
    emit transport->error(ErrorMessage);

    // Confirm the correct signals were emitted
    QCOMPARE(stateChangedSpy.count(), 1);
    QCOMPARE(stateChangedSpy.at(0).at(0), QVariant::fromValue(Transfer::Failed));
    QCOMPARE(errorChangedSpy.count(), 1);
    QCOMPARE(errorChangedSpy.at(0).at(0), QVariant(ErrorMessage));

    // Confirm the transfer status changed
    QCOMPARE(transfer.state(), Transfer::Failed);
    QCOMPARE(transfer.error(), ErrorMessage);
}

QTEST_MAIN(TestTransfer)
#include "TestTransfer.moc"
