/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Nathan Osman
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

#include <QtGlobal>

#if defined(Q_OS_WIN32)
#  include <windows.h>
#elif defined(Q_OS_UNIX)
#  include <sys/stat.h>
#  include <utime.h>
#endif

#include <QDateTime>

#include "file.h"

File::File(const QString &root, const QVariantMap &properties)
{
    mRelativeFilename = properties.value("name").toString();

    // Create the full path by combining the root with the relative filename
    mFile.setFileName(QDir::cleanPath(root + QDir::separator() + mRelativeFilename));

    mSize = properties.value("size").toLongLong();

    mReadOnly = properties.value("readOnly").toBool();
    mExecutable = properties.value("executable").toBool();

    // Suppport older versions of NitroShare that send the last modification
    // and last read properties with different names

    mCreated = properties.value("created").toLongLong();
    mLastRead = properties.value("lastRead",
        properties.value("last_read").toLongLong()).toLongLong();
    mLastModified = properties.value("lastModified",
        properties.value("last_modified").toLongLong()).toLongLong();
}

File::File(const QDir &root, const QFileInfo &info, int blockSize)
{
    mFile.setFileName(info.absoluteFilePath());
    mBlockSize = blockSize;

    mRelativeFilename = root.relativeFilePath(info.absoluteFilePath());

    mSize = info.size();
    mReadOnly = !info.isWritable();
    mExecutable = info.isExecutable();

    mCreated = info.created().toMSecsSinceEpoch();
    mLastRead = info.lastRead().toMSecsSinceEpoch();
    mLastModified = info.lastModified().toMSecsSinceEpoch();
}

bool File::readOnly() const
{
    return mReadOnly;
}

bool File::executable() const
{
    return mExecutable;
}

qint64 File::created() const
{
    return mCreated;
}

qint64 File::lastRead() const
{
    return mLastRead;
}

qint64 File::lastModified() const
{
    return mLastModified;
}

qint64 File::last_read() const
{
    return mLastRead;
}

qint64 File::last_modified() const
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
    if (openMode == Write && !QDir(QFileInfo(mFile.fileName()).absolutePath()).mkpath(".")) {
        return false;
    }
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

    return data;
}

void File::write(const QByteArray &data)
{
    if (mFile.write(data) == -1) {
        emit error(mFile.errorString());
    }
}

#ifdef Q_OS_WIN32

// Adapted from https://support.microsoft.com/en-us/help/167296
void unixTimestampMsToFiletime(qint64 timestampMs, LPFILETIME pft)
{
    // Convert from MS to 100-nanosecond intervals
    quint64 ll = timestampMs * 10000 + 116444736000000000;

    // Assign to members of FILETIME struct
    pft->dwLowDateTime = (DWORD)ll;
    pft->dwHighDateTime = ll >> 32;
}

#endif

void File::close()
{
    mFile.close();

#if defined(Q_OS_WIN32)

    if (mReadOnly) {
        BOOL succeeded = SetFileAttributesW(
            reinterpret_cast<LPCWSTR>(mFile.fileName().utf16()),
            FILE_ATTRIBUTE_READONLY
        );
        if (succeeded == FALSE) {
            emit error("unable to set readonly attribute");
            return;
        }
    }

    // Windows doesn't have a concept of "executable" files

    FILETIME createdFiletime;
    FILETIME lastReadFiletime;
    FILETIME lastModifiedFiletime;

    // Perform the conversions
    unixTimestampMsToFiletime(mCreated, &createdFiletime);
    unixTimestampMsToFiletime(mLastRead, &lastReadFiletime);
    unixTimestampMsToFiletime(mLastModified, &lastModifiedFiletime);

    // Open the file
    HANDLE hFile = CreateFileW(
        reinterpret_cast<LPCWSTR>(mFile.fileName().utf16()),
        GENERIC_READ | FILE_WRITE_ATTRIBUTES,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL
    );
    if (hFile == INVALID_HANDLE_VALUE) {
        emit error(QString("unable to open %1").arg(mFile.fileName()));
        return;
    }

    // Set the attributes
    BOOL succeeded = SetFileTime(
        hFile,
        mCreated ? &createdFiletime : NULL,
        mLastRead ? &lastReadFiletime : NULL,
        mLastModified ? &lastModifiedFiletime : NULL
    );
    if (succeeded == FALSE) {
        emit error("unable to set file times");
    }

    CloseHandle(hFile);

#elif defined(Q_OS_UNIX)

    // Retrieve existing statistics
    struct stat oldStats;
    if (stat(mFile.fileName().toUtf8().constData(), &oldStats)) {
        emit error("unable to read file stats");
        return;
    }

    // Load the existing file mode
    mode_t fileMode = oldStats.st_mode;

    // If the file is marked as read-only, remove the write bits
    if (mReadOnly) {
        fileMode &= ~(S_IWUSR | S_IWGRP | S_IWOTH);
    }

    // If the file is marked as executable, add the executable bits
    if (mExecutable) {
        fileMode |= (S_IXUSR | S_IXGRP | S_IXOTH);
    }

    // If the value has changed, update the file
    if (oldStats.st_mode != fileMode &&
            chmod(mFile.fileName().toUtf8().constData(), fileMode)) {
        emit error("unable to execute chmod");
        return;
    }

    // Create the struct with the new values
    struct utimbuf newTimes;

#ifdef Q_OS_DARWIN
    time_t st_atim = oldStats.st_atimespec.tv_sec;
    time_t st_mtim = oldStats.st_mtimespec.tv_sec;
#else
    time_t st_atim = oldStats.st_atim.tv_sec;
    time_t st_mtim = oldStats.st_mtim.tv_sec;
#endif

    newTimes.actime = mLastRead ? mLastRead / 1000 : st_atim;
    newTimes.modtime = mLastModified ? mLastModified / 1000 : st_mtim;

    // Set the new values
    if (utime(mFile.fileName().toUtf8(), &newTimes)) {
        emit error("unable to set file times");
    }

#endif
}
