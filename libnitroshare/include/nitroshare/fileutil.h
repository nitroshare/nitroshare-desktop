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

#ifndef LIBNITROSHARE_FILEUTIL_H
#define LIBNITROSHARE_FILEUTIL_H

#include <nitroshare/config.h>

/**
 * @brief Filesystem utility methods
 */
class NITROSHARE_EXPORT FileUtil
{
public:

    /**
     * @brief Create a file with the specified content
     * @param filename absolute path to file
     * @param content data to write to file
     * @return true if the file was created
     */
    static bool createFile(const QString &filename, const QByteArray &content = QByteArray());

    /**
     * @brief Ensure that a filename is unique
     * @param originalFilename absolute filename used for generating a unique one
     * @return unique filename
     */
    static QString uniqueFilename(const QString &originalFilename);

    /**
     * @brief Copy a file or directory to a new location
     * @param src absolute path to source
     * @param dest absolute path to destination
     * @param overwrite true to replace the destination if it exists
     * @return true if the copy operation succeeds
     *
     * Due to the complex nature of this operation, it is not atomic. It is
     * possible for some operations to fail
     */
    static bool copy(const QString &src, const QString &dest, bool overwrite = false);
};

#endif // LIBNITROSHARE_FILEUTIL_H
