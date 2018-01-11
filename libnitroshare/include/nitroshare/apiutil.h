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

#ifndef LIBNITROSHARE_APIUTIL_H
#define LIBNITROSHARE_APIUTIL_H

#include <QVariant>
#include <QVariantMap>

#include <nitroshare/config.h>

/**
 * @brief Utility methods for interacting with the local API
 */
class NITROSHARE_EXPORT ApiUtil
{
public:

    /**
     * @brief Send a request to the local API
     * @param action name of action to invoke
     * @param params parameters for the action
     * @param returnVal return value upon successful completion
     * @param error pointer to a string that will contain an error description
     * @return true if the request completed without error
     *
     * This method is synchronous and will block until finished. This is rarely
     * a problem since network delays should be nonexistent. A timeout is still
     * used, however, to ensure the request does not hang.
     */
    static bool sendRequest(const QString &action,
                            const QVariantMap &params,
                            QVariant &returnVal,
                            QString *error = nullptr);

    /**
     * @brief Determine if an existing instance of NitroShare is running
     * @return true if NitroShare is already running
     */
    static bool isRunning();
};

#endif // LIBNITROSHARE_APIUTIL_H
