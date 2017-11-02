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

#include <QFile>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

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
