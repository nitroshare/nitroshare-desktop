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

#ifndef LIBNITROSHARE_HANDLER_H
#define LIBNITROSHARE_HANDLER_H

#include <QString>
#include <QVariantMap>

#include "config.h"

class Item;

/**
 * @brief Handler for creating items that can be sent and received in a transfer
 *
 * In order to transfer a specific type of item (such as a file or directory),
 * a class that derives from Handler must be registered with
 * Application::registerHandler(). The handler's methods will be invoked as
 * necessary to create Item instances for items in a bundle.
 */
class NITROSHARE_EXPORT Handler
{
public:

    /**
     * @brief Create an item for the specified type
     * @param type identifier
     * @param properties map of properties
     * @return newly created item
     */
    virtual Item *createItem(const QString &type, const QVariantMap &properties) = 0;
};

#endif // LIBNITROSHARE_HANDLER_H
