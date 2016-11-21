/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Nathan Osman
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

#include "file.h"

const QString TypeValue = "file";

const QString ReadOnlyKey = "read_only";
const QString CreatedMsKey = "created_ms";
const QString LastReadMsKey = "last_read_ms";
const QString LastModifiedMsKey = "last_modified_ms";

File::File()
    : mSize(0),
      mReadOnly(false)
{
}

File::File(const QDir &root, const QFileInfo &info)
{
    mLocalFilename = info.absoluteFilePath();
    mRelativeFilename = root.relativeFilePath(mLocalFilename);

    mSize = info.size();
    mReadOnly = !info.isWritable();

    mCreated = info.created();
    mLastRead = info.lastRead();
    mLastModified = info.lastModified();
}

QString File::type() const
{
    return TypeValue;
}

QVariantMap File::properties() const
{
    return {
        { TypeKey, "file" },
        { NameKey, mRelativeFilename },
        { SizeKey, mSize },
        { ReadOnlyKey, mReadOnly },
        { CreatedMsKey, mCreated.toMSecsSinceEpoch() },
        { LastReadMsKey, mLastRead.toMSecsSinceEpoch() },
        { LastModifiedMsKey, mLastModified.toMSecsSinceEpoch() }
    };
}

void File::setProperties(const QVariantMap &properties)
{
    mRelativeFilename = properties.value(NameKey).toString();

    mSize = properties.value(SizeKey).toLongLong();
    mReadOnly = properties.value(ReadOnlyKey).toBool();

    mCreated = QDateTime::fromMSecsSinceEpoch(properties.value(CreatedMsKey).toInt());
    mLastRead = QDateTime::fromMSecsSinceEpoch(properties.value(LastReadMsKey).toInt());
    mLastModified = QDateTime::fromMSecsSinceEpoch(properties.value(LastModifiedMsKey).toInt());
}

QIODevice *File::createReader()
{
    return nullptr;
}

QIODevice *File::createWriter()
{
    return nullptr;
}
