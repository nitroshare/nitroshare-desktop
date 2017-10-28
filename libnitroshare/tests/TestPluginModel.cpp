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

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QSignalSpy>
#include <QTest>

#include <nitroshare/application.h>
#include <nitroshare/pluginmodel.h>

class TestPluginModel : public QObject
{
    Q_OBJECT

public:

    TestPluginModel();

private slots:

    void testSignals();

private:

    QString mPluginPath;
};

TestPluginModel::TestPluginModel()
    : mPluginPath(QDir::cleanPath(
          QFileInfo(QCoreApplication::arguments().at(0)).absolutePath() +
          QDir::separator() + "plugins"
      ))
{
}

void TestPluginModel::testSignals()
{
    Application application;

    // Load the "dummy" plugins from the directory passed via env. variable and
    // ensure that a signal was emitted when they were loaded
    QSignalSpy rowsInsertedSpy(application.pluginModel(), &PluginModel::rowsInserted);
    application.pluginModel()->loadPluginsFromDirectories({ mPluginPath });
    QCOMPARE(rowsInsertedSpy.count(), 2);

    // Unload the plugins and make sure the correct signal was emitted
    QSignalSpy dataChangedSpy(application.pluginModel(), &PluginModel::dataChanged);
    application.pluginModel()->unloadAll();
    QCOMPARE(dataChangedSpy.count(), 2);
}

QTEST_MAIN(TestPluginModel)
#include "TestPluginModel.moc"
