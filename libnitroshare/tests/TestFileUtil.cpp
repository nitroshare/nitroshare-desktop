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
#include <QFile>
#include <QTemporaryDir>
#include <QTest>

#include <nitroshare/fileutil.h>

const QByteArray TestContent = "content";

class TestFileUtil : public QObject
{
    Q_OBJECT

private slots:

    void testCreateFile();
    void testUniqueFilename();
    void testCopyFile();
    void testCopyDirectory();
};

void TestFileUtil::testCreateFile()
{
    QTemporaryDir dir;
    QString filename = dir.filePath("test");

    // Create a file and confirm it exists
    QVERIFY(FileUtil::createFile(filename, TestContent));
    QVERIFY(QFile::exists(filename));

    // Verify the content
    QFile file(filename);
    QVERIFY(file.open(QIODevice::ReadOnly));
    QCOMPARE(file.readAll(), TestContent);
}

void TestFileUtil::testUniqueFilename()
{
    QTemporaryDir dir;
    QString originalFilename = dir.filePath("test.tar.gz");
    QString uniqueFilename = dir.filePath("test-2.tar.gz");

    // Create an empty file
    QVERIFY(FileUtil::createFile(originalFilename));

    // Confirm that a unique filename was chosen
    QCOMPARE(FileUtil::uniqueFilename(originalFilename), uniqueFilename);
}

void TestFileUtil::testCopyFile()
{
    QTemporaryDir dir;
    QString srcFilename = dir.filePath("src");
    QString destFilename = dir.filePath("dest");

    // Create the source file with content
    QVERIFY(FileUtil::createFile(srcFilename, TestContent));

    // Copy the file
    QVERIFY(FileUtil::copy(srcFilename, destFilename));

    // Verify the content
    QFile file(destFilename);
    QVERIFY(file.open(QIODevice::ReadOnly));
    QCOMPARE(file.readAll(), TestContent);
}

void TestFileUtil::testCopyDirectory()
{
    QTemporaryDir dir;

    // Build the directory structure
    QDir srcParent(dir.filePath("src"));
    QVERIFY(srcParent.mkpath("."));
    QString srcChild = srcParent.absoluteFilePath("child");
    QVERIFY(FileUtil::createFile(srcChild));

    // Perform the copy
    QDir destParent(dir.filePath("dest"));
    QVERIFY(FileUtil::copy(srcParent.absolutePath(), destParent.absolutePath()));

    // Verify the file exists in the destination
    QVERIFY(QFile::exists(destParent.absoluteFilePath("child")));
}

QTEST_MAIN(TestFileUtil)
#include "TestFileUtil.moc"
