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
#include <nitroshare/handler.h>
#include <nitroshare/handlerregistry.h>
#include <nitroshare/item.h>
#include <nitroshare/packet.h>
#include <nitroshare/transfer.h>
#include <nitroshare/transport.h>

const QString TestDeviceName = "Test";
const QString TestItemName = "test.txt";
const QString TestItemType = "dummy";
const QByteArray TestItemData = "test";

const QJsonObject TestTransferHeader{
    { "name", TestDeviceName },
    { "size", QString::number(TestItemData.size()) },
    { "count", QString::number(1) }
};

const QJsonObject TestItemHeader{
    { "name", TestItemName },
    { "type", TestItemType },
    { "size", QString::number(TestItemData.size()) }
};

class DummyItem : public Item
{
    Q_OBJECT

public:

    explicit DummyItem(const QString &name, qint64 size) : mName(name), mSize(size) {}

    virtual QString type() const { return TestItemType; }
    virtual QString name() const { return mName; }
    virtual qint64 size() const { return mSize; }
    virtual void write(const QByteArray &data) { mData.append(data); }

    QByteArray data() const { return mData; }

private:

    QString mName;
    qint64 mSize;
    QByteArray mData;
};

class DummyHandler : public Handler
{
    Q_OBJECT

public:

    virtual QString name() const { return TestItemType; }
    virtual Item *createItem(const QString &type, const QVariantMap &params);
};

Item *DummyHandler::createItem(const QString &, const QVariantMap &params)
{
    return new DummyItem(
        params.value("name").toString(),
        params.value("size").toString().toLongLong()
    );
}

class DummyTransport : public Transport
{
    Q_OBJECT

public:

    DummyTransport() : transportClosed(false) {}

    virtual void sendPacket(Packet *packet);
    virtual void close();

    void writeData(Packet::Type type, const QByteArray &data);

    QList<QByteArray> sentPackets;
    bool transportClosed;
};

void DummyTransport::sendPacket(Packet *packet)
{
    sentPackets.append(packet->content());
}

void DummyTransport::close()
{
    transportClosed = true;
}

void DummyTransport::writeData(Packet::Type type, const QByteArray &data)
{
    Packet packet(type, data);
    emit packetReceived(&packet);
}

class TestTransfer : public QObject
{
    Q_OBJECT

private slots:

    void testReceiving();
};

void TestTransfer::testReceiving()
{
    Application application;
    DummyHandler dummyHandler;
    application.handlerRegistry()->add(&dummyHandler);

    DummyTransport *transport = new DummyTransport;
    Transfer *transfer = new Transfer(&application, transport);

    QCOMPARE(transfer->state(), Transfer::InProgress);

    // Send the transfer header to the transport
    transport->writeData(Packet::Json, QJsonDocument(TestTransferHeader).toJson());

    QCOMPARE(transfer->deviceName(), TestDeviceName);

    // Send the item header to the transport followed by the data for the item
    transport->writeData(Packet::Json, QJsonDocument(TestItemHeader).toJson());
    transport->writeData(Packet::Binary, TestItemData);

    QTRY_COMPARE(transfer->state(), Transfer::Succeeded);
}

QTEST_MAIN(TestTransfer)
#include "TestTransfer.moc"
