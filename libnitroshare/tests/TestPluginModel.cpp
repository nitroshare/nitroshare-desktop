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
#include <nitroshare/plugin.h>
#include <nitroshare/pluginmodel.h>

class TestPluginModel : public QObject
{
    Q_OBJECT

private slots:

    void testSignals();
    void testDependencies();

private:

    Plugin *loadPlugin(const QString &name);
};

void TestPluginModel::testSignals()
{
    Application application;
    Plugin *plugin = loadPlugin("dummy");

    // Ensure the rowsInserted() signal is emitted when a plugin is added
    QSignalSpy rowsInsertedSpy(application.pluginModel(), &PluginModel::rowsInserted);
    QVERIFY(application.pluginModel()->add(plugin));
    QCOMPARE(rowsInsertedSpy.count(), 1);

    // Ensure that the dataChanged() signal is emitted when the plugin is loaded
    QSignalSpy dataChangedSpy(application.pluginModel(), &PluginModel::dataChanged);
    QVERIFY(application.pluginModel()->load(plugin));
    QCOMPARE(dataChangedSpy.count(), 1);

    // Ensure that the dataChanged() signal is emitted when the plugin is unloaded
    QVERIFY(application.pluginModel()->unload(plugin));
    QCOMPARE(dataChangedSpy.count(), 2);
}

void TestPluginModel::testDependencies()
{
    Application application;
    Plugin *dummy = loadPlugin("dummy");
    Plugin *dummy2 = loadPlugin("dummy2");

    // Add only the second plugin (which depends on the first)
    QVERIFY(application.pluginModel()->add(dummy2));

    // Loading the second plugin should fail since a dependency is missing
    QVERIFY(!application.pluginModel()->load(dummy2));

    // Add the first plugin and try again (it should succeed)
    QVERIFY(application.pluginModel()->add(dummy));
    QVERIFY(application.pluginModel()->load(dummy2));

    // Unload the parent plugin and the child should be unloaded as well
    QVERIFY(application.pluginModel()->unload(dummy));
    QVERIFY(!dummy2->isLoaded());
}

Plugin *TestPluginModel::loadPlugin(const QString &name)
{
    QString filename = QDir::cleanPath(
        QFileInfo(QCoreApplication::arguments().at(0)).absolutePath() +
        QDir::separator() + "plugins" + QDir::separator() + name
    );
    return new Plugin(filename);
}

QTEST_MAIN(TestPluginModel)
#include "TestPluginModel.moc"
