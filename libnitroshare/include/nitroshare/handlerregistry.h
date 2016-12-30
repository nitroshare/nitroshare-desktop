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

#ifndef LIBNITROSHARE_HANDLERREGISTRY_H
#define LIBNITROSHARE_HANDLERREGISTRY_H

#include <QObject>

#include <nitroshare/config.h>

class Handler;

class NITROSHARE_EXPORT HandlerRegistryPrivate;

/**
 * @brief Registry mapping item types to handlers
 *
 * In order to properly serialize and deserialize items, a handler must be
 * used. In order to
 */
class NITROSHARE_EXPORT HandlerRegistry : public QObject
{
    Q_OBJECT

public:

    /**
     * @brief Create a new handler registry
     * @param parent QObject
     */
    explicit HandlerRegistry(QObject *parent = nullptr);

    /**
     * @brief Add a handler for the specified type to the registry
     * @param identifier unique identifier for the type
     * @param handler pointer to Handler
     */
    void addHandler(const QString &identifier, Handler *handler);

    /**
     * @brief Remove the handler for the specified type
     * @param identifier unique identifier for the type
     */
    void removeHandler(const QString &identifier);

    /**
     * @brief Retrieve the handler for the specified type
     * @param identifer unique identifier for the type
     * @return pointer to Handler
     */
    Handler *handlerForType(const QString &identifer);

private:

    HandlerRegistryPrivate *const d;
};

#endif // LIBNITROSHARE_HANDLERREGISTRY_H
