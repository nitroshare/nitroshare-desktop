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

#include <QSignalSpy>
#include <QTest>

#include <nitroshare/device.h>
#include <nitroshare/deviceenumerator.h>
#include <nitroshare/devicemodel.h>

const QString TestUuid1 = "uuid1";
const QString TestUuid2 = "uuid2";
const QString TestName1 = "name1";
const QString TestName2 = "name2";

class DummyDevice : public Device
{
    Q_OBJECT

public:

    DummyDevice(const QString &uuid, const QString &name)
        : mUuid(uuid),
          mName(name)
    {}

    virtual QString uuid() const { return mUuid; }
    virtual QString name() const { return mName; }

private:

    QString mUuid;
    QString mName;
};

class DummyEnumerator : public DeviceEnumerator
{
    Q_OBJECT

public:

    virtual QList<Device*> devices() const { return mDevices; }

    void addDevice(Device *device)
    {
        mDevices.append(device);
        emit deviceAdded(device);
    }

private:

    QList<Device*> mDevices;
};

class TestDeviceModel : public QObject
{
    Q_OBJECT

private slots:

    void testSignals();
};

void TestDeviceModel::testSignals()
{
    DeviceModel model;

    DummyEnumerator enumerator;

    // Create a device and add it to the enumerator
    DummyDevice device1(TestUuid1, TestName1);
    enumerator.addDevice(&device1);

    // Ensure that a signal is emitted when the enumerator is added
    QSignalSpy rowsInsertedSpy(&model, &DeviceModel::rowsInserted);
    model.addDeviceEnumerator(&enumerator);
    QCOMPARE(rowsInsertedSpy.count(), 1);

    // Ensure that a signal is emitted when a second device is added
    DummyDevice device2(TestUuid2, TestName2);
    enumerator.addDevice(&device2);
    QCOMPARE(rowsInsertedSpy.count(), 2);

    // Ensure that a signal is emitted when the enumerator is removed
    QSignalSpy rowsRemovedSpy(&model, &DeviceModel::rowsRemoved);
    model.removeDeviceEnumerator(&enumerator);
    QCOMPARE(rowsRemovedSpy.count(), 2);
}

QTEST_MAIN(TestDeviceModel)
#include "TestDeviceModel.moc"
