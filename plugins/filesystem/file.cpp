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

File::File(const QVariantMap &properties)
{
    // TODO: set mFile

    mRelativeFilename = properties.value("name").toString();

    mSize = properties.value("size").toLongLong();
    mReadOnly = properties.value("read_only").toBool();
    mExecutable = properties.value("executable").toBool();

    mCreated = QDateTime::fromMSecsSinceEpoch(properties.value("created").toInt());
    mLastRead = QDateTime::fromMSecsSinceEpoch(properties.value("last_read").toInt());
    mLastModified = QDateTime::fromMSecsSinceEpoch(properties.value("last_modified").toInt());
}

File::File(const QDir &root, const QFileInfo &info, int blockSize)
{
    mFile.setFileName(info.absoluteFilePath());
    mBlockSize = blockSize;

    mRelativeFilename = root.relativeFilePath(info.absoluteFilePath());

    mSize = info.size();
    mReadOnly = !info.isWritable();
    mExecutable = info.isExecutable();

    mCreated = info.created();
    mLastRead = info.lastRead();
    mLastModified = info.lastModified();
}

bool File::readOnly() const
{
    return mReadOnly;
}

bool File::executable() const
{
    return mExecutable;
}

QDateTime File::created() const
{
    return mCreated;
}

QDateTime File::lastRead() const
{
    return mLastRead;
}

QDateTime File::lastModified() const
{
    return mLastModified;
}

QString File::type() const
{
    return "file";
}

QString File::name() const
{
    return mRelativeFilename;
}

qint64 File::size() const
{
    return mSize;
}

bool File::open(OpenMode openMode)
{
    return mFile.open(openMode == Read ? QIODevice::ReadOnly : QIODevice::WriteOnly);
}

QByteArray File::read()
{
    // Allocate a full block and then resize to actual data length
    QByteArray data;
    data.resize(mBlockSize);
    qint64 bytesRead = mFile.read(data.data(), mBlockSize);
    data.resize(bytesRead);

    if (bytesRead == -1) {
        emit error(mFile.errorString());
    }
}

void File::write(const QByteArray &data)
{
    if (mFile.write(data) == -1) {
        emit error(mFile.errorString());
    }
}

void File::close()
{
    mFile.close();

    // TODO: set attributes
}
