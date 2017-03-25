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

#ifndef DNSBITMAP_H
#define DNSBITMAP_H

#include <QtGlobal>

/**
 * @brief 256-bit bitmap
 */
class DnsBitmap
{
public:

    /**
     * @brief Create an uninitialized (empty) bitmap
     */
    DnsBitmap();

    /**
     * @brief Retrieve the length of the bitmap
     */
    quint8 length() const;

    /**
     * @brief Retrieve a pointer to bitmap data
     */
    const quint8* data() const;

    /**
     * @brief Set bitmap data
     */
    void setData(quint8 length, const quint8 *data);

private:

    quint8 mLength;
    quint8 mData[32];
};

#endif // DNSBITMAP_H
