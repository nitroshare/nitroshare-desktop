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

const QString Key1 = "key1";
const QString Key2 = "key2";
const QString Value = "value1";

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

    void testSetGet();
    void testNotification();
    void testGroup();

private:

    Settings *newSettings();

    QTemporaryDir dir;
    int counter;
};

TestSettings::TestSettings()
    : counter(0)
{
}

void TestSettings::testSetGet()
{
    QScopedPointer<Settings> settings(newSettings());

    // Set a value
    settings->set(Key1, Value);

    // Confirm that it can be retrieved
    QCOMPARE(settings->get(Key1, &empty).toString(), Value);
}

void TestSettings::testNotification()
{
    QScopedPointer<Settings> settings(newSettings());

    // Watch for notifications
    QSignalSpy spy(settings.data(), &Settings::settingsChanged);

    // Get a value that does not exist - no signal
    settings->get(Key1, &empty);
    QCOMPARE(spy.count(), 0);

    // Set a value that is identical - no signal
    settings->set(Key1, empty());
    QCOMPARE(spy.count(), 0);

    // Set a value that is different - signal
    settings->set(Key1, Value);
    QCOMPARE(spy.count(), 1);
}

void TestSettings::testGroup()
{
    QScopedPointer<Settings> settings(newSettings());

    // Watch for notifications
    QSignalSpy spy(settings.data(), &Settings::settingsChanged);
    settings->begin();

    // Set some values and ensure no signals were emitted
    settings->set(Key1, Value);
    settings->set(Key2, Value);
    QCOMPARE(spy.count(), 0);

    // End the group and ensure a signal is emitted
    settings->end();
    QCOMPARE(spy.count(), 1);
    QStringList keys = spy.at(0).at(0).toStringList();
    QVERIFY(keys.contains(Key1));
    QVERIFY(keys.contains(Key2));
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

QTEST_MAIN(TestSettings)
#include "TestSettings.moc"
