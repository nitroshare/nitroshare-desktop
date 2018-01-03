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

#include <QSettings>
#include <QSignalSpy>
#include <QTemporaryFile>
#include <QTest>

#include <nitroshare/setting.h>
#include <nitroshare/settingsregistry.h>

#define DECLARE_REGISTRY(x) \
    QTemporaryFile temporaryFile; \
    QSettings settings(temporaryFile.fileName(), QSettings::IniFormat); \
    SettingsRegistry x(&settings)

const QString Name = "name";
const QString TestValue = "test";
const QString DefaultValue = "default";

Setting TestSetting({
    { Setting::TypeKey, Setting::String },
    { Setting::NameKey, Name },
    { Setting::DefaultValueKey, DefaultValue }
});

class TestSettingsRegistry : public QObject
{
    Q_OBJECT

private slots:

    void initTestCase();

    void testAddRemove();
    void testChange();
    void testBeginEnd();

private:

    QTemporaryDir temporaryDir;
};

void TestSettingsRegistry::initTestCase()
{
    qRegisterMetaType<Setting*>();
}

void TestSettingsRegistry::testAddRemove()
{
    DECLARE_REGISTRY(registry);

    QSignalSpy settingAddedSpy(&registry, &SettingsRegistry::settingAdded);
    QSignalSpy settingRemovedSpy(&registry, &SettingsRegistry::settingRemoved);

    // Add the setting and confirm it exists
    registry.addSetting(&TestSetting);
    QVERIFY(registry.findSetting(Name));
    QCOMPARE(settingAddedSpy.count(), 1);

    // Remove the setting and confirm it does not exist
    registry.removeSetting(&TestSetting);
    QVERIFY(!registry.findSetting(Name));
    QCOMPARE(settingRemovedSpy.count(), 1);
}

void TestSettingsRegistry::testChange()
{
    DECLARE_REGISTRY(registry);

    QSignalSpy settingsChanged(&registry, &SettingsRegistry::settingsChanged);

    // Add the setting and ensure the default value is set
    registry.addSetting(&TestSetting);
    QCOMPARE(registry.value(Name), QVariant(DefaultValue));
    QCOMPARE(settingsChanged.count(), 0);

    // Change the value and ensure it is correct when read back
    registry.setValue(Name, TestValue);
    QCOMPARE(registry.value(Name), QVariant(TestValue));
    QCOMPARE(settingsChanged.count(), 1);
    QCOMPARE(settingsChanged.at(0).at(0).toStringList().count(), 1);
    QCOMPARE(settingsChanged.at(0).at(0).toStringList().at(0), Name);
}

void TestSettingsRegistry::testBeginEnd()
{
    DECLARE_REGISTRY(registry);

    QSignalSpy settingsChanged(&registry, &SettingsRegistry::settingsChanged);

    // Add the setting and start modifying a group
    registry.addSetting(&TestSetting);
    registry.begin();

    // Change the value and ensure no signals are emitted
    registry.setValue(Name, TestValue);
    QCOMPARE(settingsChanged.count(), 0);

    // End the group and ensure the signals are emitted
    registry.end();
    QCOMPARE(settingsChanged.count(), 1);
    QCOMPARE(settingsChanged.at(0).at(0).toStringList().count(), 1);
    QCOMPARE(settingsChanged.at(0).at(0).toStringList().at(0), Name);
}

QTEST_MAIN(TestSettingsRegistry)
#include "TestSettingsRegistry.moc"
