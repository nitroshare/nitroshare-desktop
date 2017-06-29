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
#include <QStack>

#include "dir.h"

bool Dir::copy(const QString &path, const QString &destPath)
{
    QDir dir(path);
    QDir dest(destPath);
    QStack<QString> stack;

    // Push the root path on the stack and then go up one level so that
    // the relative filenames will include the name of the directory
    stack.push(dir.absolutePath());
    dir.cdUp();

    while (stack.count()) {
        QString tos = stack.pop();

        foreach (QFileInfo info, QDir(tos).entryInfoList(QDir::Dirs | QDir::Files | QDir::Hidden | QDir::NoDotAndDotDot)) {
            QString destName = dest.absoluteFilePath(dir.relativeFilePath(info.absoluteFilePath()));
            if (info.isSymLink()) {
                QString relativeTarget = dest.relativeFilePath(info.symLinkTarget());
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
