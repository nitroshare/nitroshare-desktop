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
#include <QFileInfo>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QStack>

#include <nitroshare/fileutil.h>

bool FileUtil::createFile(const QString &filename, const QByteArray &content)
{
    QFile file(filename);

    // Ensure the file can be opened
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    // Attempt to write the content and close the file
    qint64 bytesWritten = file.write(content);
    file.close();

    // The operation succeeded if all content was written
    return bytesWritten == content.size();
}

QString FileUtil::uniqueFilename(const QString &originalFilename)
{
    // If the file does not exist, use the name as-is
    if (!QFile::exists(originalFilename)) {
        return originalFilename;
    }

    // Break the filename into its components
    QRegularExpression re("^(.*?)((?:\\.tar)?\\.[^\\/\\\\]*)?$");
    QRegularExpressionMatch match = re.match(originalFilename);

    // Fill in the base for the filename and its extension
    QString base, ext;
    if (match.hasMatch()) {
        base = match.captured(1);
        ext = match.captured(2);
    } else {
        base = originalFilename;
    }

    // Begin enumerating filenames until one does not exist
    int num = 2;
    QString name;
    do {
        name = QString("%1-%2%3").arg(base).arg(num++).arg(ext);
    } while (QFile::exists(name));

    // One will eventually be found (right?)
    return name;
}

bool FileUtil::copy(const QString &src, const QString &dest, bool overwrite)
{
    QString destUnique = dest;

    QFileInfo srcInfo(src);
    QFileInfo destInfo(dest);

    // If overwrite is set, remove the destination, otherwise the copy
    // operation will fail; if overwrite is not set, come up with a unique
    // name for the item (if necessary)
    if (overwrite) {
        if (destInfo.exists() &&
                ((destInfo.isDir() && !QDir(dest).removeRecursively()) ||
                 (destInfo.isFile() && !QFile::remove(dest)))) {
            return false;
        }
    } else {
        destUnique = uniqueFilename(dest);
    }

    // If the source is not a directory, simply use QFile::copy()
    if (!srcInfo.isDir()) {
        return QDir(QFileInfo(destUnique).absolutePath()).mkpath(".") &&
                QFile::copy(src, destUnique);
    }

    // Begin by creating the destination directory
    if (!QDir(destUnique).mkpath(".")) {
        return false;
    }

    // Use a stack to traverse the directory
    QDir srcDir(src);
    QDir destDir(destUnique);
    QStack<QString> stack;

    // Push the source path on the stack
    stack.push(srcDir.absolutePath());

    // While items remain on the stack, pop an item off and traverse it
    while (stack.count()) {
        QString tos = stack.pop();
        foreach (QFileInfo info, QDir(tos).entryInfoList(QDir::Dirs | QDir::Files | QDir::Hidden | QDir::NoDotAndDotDot)) {
            QString destName = destDir.absoluteFilePath(srcDir.relativeFilePath(info.absoluteFilePath()));
            if (info.isSymLink()) {
                QString relativeTarget = destDir.relativeFilePath(info.symLinkTarget());
                if (!QFile::link(relativeTarget, destName)) {
                    return false;
                }
            } else if (info.isDir()) {
                if (!QDir(destName).mkpath(".")) {
                    return false;
                }
                stack.push(info.absoluteFilePath());
            } else {
                if (!QFile::copy(info.absoluteFilePath(), destName)) {
                    return false;
                }
            }
        }
    }

    // All operations succeeded
    return true;
}
