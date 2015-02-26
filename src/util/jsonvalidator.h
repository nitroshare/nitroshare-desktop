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

#ifndef NS_JSONVALIDATOR_H
#define NS_JSONVALIDATOR_H

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

/**
 * @brief Validator for JSON documents
 *
 * There are many classes that need to ensure that JSON data received from an
 * external source contains expected data. This class exists to fill the need.
 *
 * The validation methods accept a reference to the expected type and return a
 * boolean indicating success or failure. This allows the method calls to be
 * chained in an if statement like so:
 *
 *     QJsonDocument doc = ...
 *     QJsonValue value;
 *     QJsonArray array;
 *     QString item1;
 *     qint64 item2;
 *
 *     if(validator.isArray(doc, array) &&
 *             array.count() &&
 *             validator.isObject(array.at(0), object) &&
 *             validator.objectContains(object, "item1", item1) &&
 *             validator.objectContains(object, "item2", item2) {
 *
 *         //...
 *     }
 */
class JsonValidator
{
public:

    static bool isArray(const QJsonDocument &document, QJsonArray &array);
    static bool isObject(const QJsonDocument &document, QJsonObject &object);
    static bool isObject(const QJsonValue &value, QJsonObject &object);

    static bool objectContains(const QJsonObject &object, const QString &key, QString &value);
    static bool objectContains(const QJsonObject &object, const QString &key, qint64 &value);
};

#endif // NS_JSONVALIDATOR_H
