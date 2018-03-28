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

#include <qmdnsengine/bitmap.h>

#include "bitmap_p.h"

using namespace QMdnsEngine;

BitmapPrivate::BitmapPrivate()
    : length(0),
      data(nullptr)
{
}

BitmapPrivate::~BitmapPrivate()
{
    free();
}

void BitmapPrivate::free()
{
    if (data) {
        delete data;
    }
}

void BitmapPrivate::fromData(quint8 newLength, const quint8 *newData)
{
    data = new quint8[newLength];
    for (int i = 0; i < newLength; ++i) {
        data[i] = newData[i];
    }
    length = newLength;
}

Bitmap::Bitmap()
    : d(new BitmapPrivate)
{
}

Bitmap::Bitmap(const Bitmap &other)
    : d(new BitmapPrivate)
{
    d->fromData(other.d->length, other.d->data);
}

Bitmap &Bitmap::operator=(const Bitmap &other)
{
    d->free();
    d->fromData(other.d->length, other.d->data);
    return *this;
}

bool Bitmap::operator==(const Bitmap &other)
{
    if (d->length != other.d->length) {
        return false;
    }
    for (int i = 0; i < d->length; ++i) {
        if (d->data[i] != other.d->data[i]) {
            return false;
        }
    }
    return true;
}

Bitmap::~Bitmap()
{
    delete d;
}

quint8 Bitmap::length() const
{
    return d->length;
}

const quint8 *Bitmap::data() const
{
    return d->data;
}

void Bitmap::setData(quint8 length, const quint8 *data)
{
    d->free();
    d->fromData(length, data);
}
