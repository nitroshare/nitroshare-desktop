/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Nathan Osman
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

#ifndef NS_BUNDLEITEM_H
#define NS_BUNDLEITEM_H

#include <QDateTime>
#include <QDir>
#include <QFileInfo>

/**
 * @brief An item to be transferred
 *
 * This class maintains information about a file or directory to be
 * transferred. Currently, this includes the item's name, creation, and
 * modification times.
 */
class BundleItem
{
public:

    explicit BundleItem(const QFileInfo &info) : BundleItem(QDir(info.path()), info) {}
    BundleItem(const QDir &root, const QFileInfo &info) : mRoot(root), mInfo(info) {}

    QString relativeFilename() const { return mRoot.relativeFilePath(absoluteFilename()); }
    QString absoluteFilename() const { return mInfo.absoluteFilePath(); }

    bool isDir() const { return mInfo.isDir(); }

    qint64 created() const { return mInfo.created().toMSecsSinceEpoch(); }
    qint64 lastModified() const { return mInfo.lastModified().toMSecsSinceEpoch(); }
    qint64 lastRead() const { return mInfo.lastRead().toMSecsSinceEpoch(); }

private:

    QDir mRoot;
    QFileInfo mInfo;
};

#endif // NS_BUNDLEITEM_H
