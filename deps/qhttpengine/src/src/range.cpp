/*
 * Copyright (c) 2017 Aleksei Ermakov
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

#include <QRegExp>

#include <qhttpengine/range.h>

#include "range_p.h"

using namespace QHttpEngine;

RangePrivate::RangePrivate(Range *range)
    : q(range)
{
}

Range::Range()
    : d(new RangePrivate(this))
{
    d->from = 1;
    d->to = 0;
    d->dataSize = -1;
}

Range::Range(const QString &range, qint64 dataSize)
    : d(new RangePrivate(this))
{
    QRegExp regExp("^(\\d*)-(\\d*)$");

    int from = 0, to = -1;

    if (regExp.indexIn(range.trimmed()) != -1) {
        QString fromStr = regExp.cap(1);
        QString toStr = regExp.cap(2);

        // If both strings are empty - range is invalid. Setting to out of
        // bounds range and returning.
        if (fromStr.isEmpty() && toStr.isEmpty()) {
            d->from = 1;
            d->to = 0;
            d->dataSize = -1;
            return;
        }

        bool okFrom = true, okTo = true;

        if (!fromStr.isEmpty()) {
            from = fromStr.toInt(&okFrom);
        }
        if (!toStr.isEmpty()) {
            to = toStr.toInt(&okTo);
        }

        // If failed to parse value - set to invalid range and return.
        if (!okFrom) {
            d->from = 1;
            d->to = 0;
            d->dataSize = -1;
            return;
        }

        if (!okTo) {
            d->from = 1;
            d->to = 0;
            d->dataSize = -1;
            return;
        }

        // In case of 'last N bytes' range (Ex.: "Range: bytes=-500"),
        // set from to -to and to to -1
        if (fromStr.isEmpty()) {
            from = -to;
            to = -1;
        }
    } else { // If regexp didn't match - set to invalid range and return.
        d->from = 1;
        d->to = 0;
        d->dataSize = -1;
        return;
    }

    d->from = from;
    d->to = to;
    d->dataSize = dataSize;
}

Range::Range(qint64 from, qint64 to, qint64 dataSize)
    : d(new RangePrivate(this))
{
    d->from = from;
    d->to = to < 0 ? -1 : to;
    d->dataSize = dataSize < 0 ? -1 : dataSize;
}

Range::Range(const Range &other, qint64 dataSize)
    : d(new RangePrivate(this))
{
    d->from = other.d->from;
    d->to = other.d->to;
    d->dataSize = dataSize;
}

Range::~Range()
{
    delete d;
}

Range& Range::operator=(const Range &other)
{
    if (&other != this) {
        d->from = other.d->from;
        d->to = other.d->to;
        d->dataSize = other.d->dataSize;
    }

    return *this;
}

qint64 Range::from() const
{
    // Last N bytes requested
    if (d->from < 0 && d->dataSize != -1) {
        // Check if data is smaller then requested range
        if (- d->from >= d->dataSize) {
            return 0;
        }
        return d->dataSize + d->from;
    }

    // Check if d->from is bigger than d->to or d->dataSize
    if ((d->from > d->to && d->to != -1) ||
            (d->from >= d->dataSize && d->dataSize != -1)) {
        return 0;
    }

    return d->from;
}

qint64 Range::to() const
{
    // Last N bytes requested
    if (d->from < 0 && d->dataSize != -1) {
        return d->dataSize - 1;
    }

    // Skip first N bytes requested
    if (d->from > 0 && d->to == -1 && d->dataSize != -1) {
        return d->dataSize - 1;
    }

    // Check if d->from is bigger then d->to
    if (d->from > d->to && d->to != -1) {
        return d->from;
    }

    // When d->to overshoots dataSize
    if ((d->to >= d->dataSize || d->to == -1) && d->dataSize != -1) {
        return d->dataSize - 1;
    }

    return d->to;
}

qint64 Range::length() const
{
    if (!isValid()) {
        return -1;
    }

    // Last n bytes
    if (d->from < 0) {
        return -(d->from);
    }

    // From and to are set
    if (d->to >= 0) {
        return d->to - d->from + 1;
    }

    // From to to end
    if (d->dataSize >= 0) {
        return d->dataSize - d->from;
    }

    return -1;
}

qint64 Range::dataSize() const
{
    return d->dataSize;
}

bool Range::isValid() const
{
    // Valid cases:
    // 1. "-500/1000"   => from: -500, to:  -1; dataSize: 1000
    // 2. "10-/1000"    => from:   10, to:  -1; dataSize: 1000
    // 3. "10-600/1000" => from:   10, to: 600; dataSize: 1000
    // 4. "-500/*"      => from: -500, to:  -1; dataSize:   -1
    // 5. "10-/*"       => from:   10, to:  -1; dataSize:   -1
    // 6. "10-600/*"    => from:   10, to: 600; dataSize:   -1

    // DataSize is set
    if (d->dataSize >= 0) {
        if (d->from < 0) { // Last n bytes
            // Check if from is in range of dataSize
            if (d->dataSize + d->from >= 0) {
                return true;
            }
        } else {
            if (d->to <= -1) { // To isn't set, range is up to the end
                // Check if from is in range of dataSize
                if (d->from < d->dataSize) {
                    return true;
                }
            } else { // from, to and dataSize are set
                if (d->from <= d->to && d->to < d->dataSize) {
                    return true;
                }
            }
        }
    } else { // dataSize is not set
        if (d->from < 0) { // Last n bytes
            return true;
        } else {
            if (d->to <= -1) { // To isn't set, range is up to the end
                return true;
            } else { // from and to are set
                if (d->from <= d->to) {
                    return true;
                }
            }
        }
    }

    return false;
}

QString Range::contentRange() const
{
    QString fromStr, toStr, sizeStr = "*";

    if (d->dataSize >= 0) {
        if (isValid()) {
            fromStr = QString::number(from());
            toStr = QString::number(to());
            sizeStr = QString::number(dataSize());
        } else {
            sizeStr = QString::number(dataSize());
        }
    } else {
        if (isValid()) {
            fromStr = QString::number(from());
            toStr = QString::number(to());
        } else {
            return "";
        }
    }

    if (fromStr.isEmpty() || toStr.isEmpty()) {
        return QString("*/%1").arg(sizeStr);
    }

    return QString("%1-%2/%3").arg(fromStr, toStr, sizeStr);
}
