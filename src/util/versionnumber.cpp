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

#include <QStringList>

#include "versionnumber.h"

bool VersionNumber::isGreaterThan(const QString &version1, const QString &version2)
{
    QStringList components1 = version1.split('.');
    QStringList components2 = version2.split('.');

    for(int i = 0; i < components1.count(); ++i) {

        // If the second version doesn't contain this
        // many items, then the first is greater
        if(i == components2.count()) {
            return true;
        }

        // Convert the two components to integers
        int num1 = components1.at(i).toInt();
        int num2 = components2.at(i).toInt();

        if(num1 > num2) {
            return true;
        } else if (num1 < num2) {
            return false;
        }

        // Continue to the next component if the two are equal
    }

    // If we reach this point, either the two versions are equal or the second
    // version contains more components - either way, the first is not greater
    return false;
}
