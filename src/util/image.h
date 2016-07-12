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

#ifndef NS_IMAGE_H
#define NS_IMAGE_H

#include <QPixmap>
#include <QRect>
#include <QWidget>

/**
 * @brief Utility methods for working with images
 */
class Image
{
public:

    /**
     * @brief Render an SVG file to the provided pixmap sized appropriately for the given widget
     * @param filename SVG file
     * @param widget used for determining the correct size
     * @return pixmap with rendered content
     */
    static QPixmap renderSvg(const QString &filename, QWidget *widget);

    /**
     * @brief Render an SVG file to the provided pixmap using the given size
     * @param filename SVG file
     * @param rect target rect for the pixmap
     * @return pixmap with rendered content
     */
    static QPixmap renderSvg(const QString &filename, const QRect &rect);
};

#endif // NS_IMAGE_H
