/**
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
 **/

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QStack>

#include "filesystem.h"

bool Filesystem::copyDirectory(const QString &src, const QString &dest, bool overwrite)
{
    QString destUnique;

    // If overwrite is set, remove the destination; otherwise, ensure that it
    // is assigned a unique name
    if (overwrite) {
        QDir dir(destUnique = dest);
        if (dir.exists() && !dir.removeRecursively()) {
            return false;
        }
    } else {
        destUnique = uniqueFilename(dest);
        if (!QDir(dest).mkpath(".")) {
            return false;
        }
    }

    QDir srcDir(src);
    QDir destDir(destUnique);
    QStack<QString> stack;

    // Push the source path on the stack
    stack.push(srcDir.absolutePath());

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

    return true;
}

QString Filesystem::uniqueFilename(const QString &originalFilename)
{
    // If the file does not exist, we're already done
    if (!QFile::exists(originalFilename)) {
        return originalFilename;
    }

    // Break the filename into its components
    QRegularExpression re("^(.*?)((?:\\.tar)?\\.[^\\/\\\\]*)?$");
    QRegularExpressionMatch match = re.match(originalFilename);

    // Fill in the base for the filename and its extension
    QString base, ext;
    if (match.hasMatch()) {
        base = originalFilename;
    } else {
        base = match.captured(1);
        ext = match.captured(2);
    }

    // Begin enumerating filenames until one does not exist
    int num = 2;
    QString name;
    do {
        name = QString("%1-%2%3").arg(base).arg(num++).arg(ext);
    } while (QFile::exists(name));

    // One will eventually be found
    return name;
}
