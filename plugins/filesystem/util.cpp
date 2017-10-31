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
#include <QFileInfo>
#include <QList>
#include <QStack>

#include <nitroshare/bundle.h>

#include "file.h"
#include "util.h"

const int BlockSize = 65536;

Bundle *Util::createBundle(const QStringList &items)
{
    Bundle *bundle = new Bundle;
    foreach (const QString &item, items) {
        QFileInfo info(item);
        if (info.isFile()) {
            addFile(bundle, item);
        } else if (info.isDir()) {
            addDirectory(bundle, item);
        }
    }
    return bundle;
}

void Util::addFile(Bundle *bundle, const QString &path)
{
    QFileInfo info(path);
    bundle->add(new File(info.absolutePath(), info, BlockSize));
}

void Util::addDirectory(Bundle *bundle, const QString &path)
{
    // Although enumerating directory contents recursively might be more
    // intuitive, doing this in a loop with a stack is much faster

    QDir root(path);
    QStack<QString> stack;

    // Push the root path on the stack and then go up one level so that
    // the relative filenames will include the name of the directory
    stack.push(root.absolutePath());
    root.cdUp();

    while (stack.count()) {
        QList<QFileInfo> infoList = QDir(stack.pop()).entryInfoList(
            QDir::Dirs | QDir::Files | QDir::Hidden | QDir::NoDotAndDotDot | QDir::NoSymLinks
        );
        foreach (const QFileInfo &info, infoList) {
            if (info.isDir()) {
                stack.push(info.absolutePath());
            } else {
                bundle->add(new File(root, info, BlockSize));
            }
        }
    }
}
