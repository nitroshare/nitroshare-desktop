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

const QString TestUuid = "uuid";
const QString TestName = "name";

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

    void addDevice(Device *device) { emit deviceAdded(device); }
    void removeDevice(Device *device) { emit deviceRemoved(device); }

    virtual QString name() const { return "dummy"; }
};

class TestDeviceModel : public QObject
{
    Q_OBJECT

private slots:

    void testSignals();
    void testEnumerator();
};

void TestDeviceModel::testSignals()
{
    DeviceModel model;
    DummyEnumerator enumerator;
    model.addDeviceEnumerator(&enumerator);

    // Ensure that a signal is emitted when a device is added
    QSignalSpy rowsInsertedSpy(&model, &DeviceModel::rowsInserted);
    DummyDevice device(TestUuid, TestName);
    enumerator.addDevice(&device);
    QCOMPARE(rowsInsertedSpy.count(), 1);

    // Ensure that a signal is emitted when a device is removed
    QSignalSpy rowsRemovedSpy(&model, &DeviceModel::rowsRemoved);
    enumerator.removeDevice(&device);
    QCOMPARE(rowsRemovedSpy.count(), 1);
}

void TestDeviceModel::testEnumerator()
{
    DeviceModel model;
    DummyEnumerator enumerator;
    model.addDeviceEnumerator(&enumerator);
    DummyDevice device(TestUuid, TestName);
    enumerator.addDevice(&device);

    // Ensure that removing the enumerator results in devices being removed
    QCOMPARE(model.rowCount(), 1);
    model.removeDeviceEnumerator(&enumerator);
    QCOMPARE(model.rowCount(), 0);
}

QTEST_MAIN(TestDeviceModel)
#include "TestDeviceModel.moc"
