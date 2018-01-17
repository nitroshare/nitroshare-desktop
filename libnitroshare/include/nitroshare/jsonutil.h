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

#ifndef LIBNITROSHARE_JSONUTIL_H
#define LIBNITROSHARE_JSONUTIL_H

#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
#include <QObject>

#include <nitroshare/config.h>

/**
 * @brief JSON utility methods
 */
class NITROSHARE_EXPORT JsonUtil
{
public:

    /**
     * @brief Create a JSON object from the properties in an object
     * @param object pointer to QObject
     * @return JSON object with properties
     *
     * Note: all properties that are 64-bit integers will be converted to
     * strings to prevent data loss.
     */
    static QJsonObject objectToJson(QObject *object);

    /**
     * @brief Convert the provided JSON value to a byte array
     * @param value JSON value
     * @return byte array
     */
    static QByteArray jsonValueToByteArray(const QJsonValue &value);

    /**
     * @brief Convert a byte array to its equivalent JSON value
     * @param data byte array
     * @param parseError pointer to QJsonParseError
     * @return JSON value
     */
    static QJsonValue byteArrayToJsonValue(const QByteArray &data, QJsonParseError *parseError = nullptr);
};

#endif // LIBNITROSHARE_JSONUTIL_H
