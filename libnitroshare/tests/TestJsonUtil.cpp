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

#include <QJsonObject>
#include <QTest>

#include <nitroshare/jsonutil.h>

const QString ParentValue = "parent";
const QString ChildValue = "child";

class Parent : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString parent READ parent)

public:

    QString parent() const { return ParentValue; }
};

class Child : public Parent
{
    Q_OBJECT
    Q_PROPERTY(QString child READ child)

public:

    QString child() const { return ChildValue; }
};

class TestJsonUtil : public QObject
{
    Q_OBJECT

private slots:

    void testObjectToJson();
};

void TestJsonUtil::testObjectToJson()
{
    Child child;
    QJsonObject object = JsonUtil::objectToJson(&child);
    QJsonObject referenceObject{
        { "parent", ParentValue },
        { "child", ChildValue }
    };
    QCOMPARE(object, referenceObject);
}

QTEST_MAIN(TestJsonUtil)
#include "TestJsonUtil.moc"
