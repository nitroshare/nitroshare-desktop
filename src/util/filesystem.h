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

#ifndef NS_DIR_H
#define NS_DIR_H

#include <QString>

class Filesystem
{
public:

    /**
     * @brief Copy a directory and its contents
     * @param src absolute path to source
     * @param dest absolute path to destination
     * @param overwrite true to replace the destination if it exists
     * @return true if the copy operation succeeded
     */
    static bool copyDirectory(const QString &src, const QString &dest, bool overwrite);

    /**
     * @brief Ensure that a filename is unique
     * @param originalFilename base for generating a unique filename
     * @return unique filename
     */
    static QString uniqueFilename(const QString &originalFilename);
};

#endif // NS_DIR_H
