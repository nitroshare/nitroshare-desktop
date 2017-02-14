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

#include <QDir>
#include <QScopedPointer>
#include <QSettings>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QTest>

#include <nitroshare/settings.h>

const QString Key = "key";
const QString Value1 = "value1";
const QString Value2 = "value2";

QVariant empty()
{
    return QVariant();
}

class TestSettings : public QObject
{
    Q_OBJECT

public:

    TestSettings();

private slots:

    void testAddChangeRemove();
    void testBeginEnd();

private:

    Settings *newSettings();
    bool spyContainsKey(QSignalSpy *spy);

    QTemporaryDir dir;
    int counter;
};

TestSettings::TestSettings()
    : counter(0)
{
}

void TestSettings::testAddChangeRemove()
{
    QScopedPointer<Settings> settings(newSettings());

    // Ensure a signal is emitted when a new setting is added
    QSignalSpy addSpy(settings.data(), &Settings::settingsAdded);
    settings->addSetting(Key, QVariantMap{{Settings::DefaultKey, Value1}});
    QVERIFY(spyContainsKey(&addSpy));

    // Ensure the default value was set
    QCOMPARE(settings->value(Key).toString(), Value1);

    // Ensure *no* signal is emitted when the value does not change
    QSignalSpy changeSpy(settings.data(), &Settings::settingsChanged);
    settings->setValue(Key, Value1);
    QCOMPARE(changeSpy.count(), 0);

    // Ensure a signal is emitted when the value *does* change
    settings->setValue(Key, Value2);
    QVERIFY(spyContainsKey(&changeSpy));

    // Ensure the new value is set
    QCOMPARE(settings->value(Key).toString(), Value2);

    // Ensure a signal is emitted when the setting is removed
    QSignalSpy removeSpy(settings.data(), &Settings::settingsRemoved);
    settings->removeSetting(Key);
    QVERIFY(spyContainsKey(&removeSpy));
}

void TestSettings::testBeginEnd()
{
    QScopedPointer<Settings> settings(newSettings());
    settings->begin();

    QSignalSpy addSpy(settings.data(), &Settings::settingsAdded);
    QSignalSpy changeSpy(settings.data(), &Settings::settingsChanged);
    QSignalSpy removeSpy(settings.data(), &Settings::settingsRemoved);

    // Perform operations that would normally cause signals to be emitted
    settings->addSetting(Key);
    settings->setValue(Key, Value1);
    settings->removeSetting(Key);

    // Ensure *no* signals were emitted
    QCOMPARE(addSpy.count(), 0);
    QCOMPARE(changeSpy.count(), 0);
    QCOMPARE(removeSpy.count(), 0);

    settings->end();

    // Ensure signals were emitted for all three
    QVERIFY(spyContainsKey(&addSpy));
    QVERIFY(spyContainsKey(&changeSpy));
    QVERIFY(spyContainsKey(&removeSpy));
}

Settings *TestSettings::newSettings()
{
    QSettings *s = new QSettings(
        QDir(dir.path()).absoluteFilePath(QString("%1").arg(counter++)),
        QSettings::IniFormat
    );
    Settings *settings = new Settings(s);
    s->setParent(settings);
    return settings;
}

bool TestSettings::spyContainsKey(QSignalSpy *spy)
{
    return spy->count() == 1 &&
        spy->value(0).count() == 1 &&
        spy->value(0).at(0).toStringList().contains(Key);
}

QTEST_MAIN(TestSettings)
#include "TestSettings.moc"
