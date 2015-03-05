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

#ifndef NS_JSON_H
#define NS_JSON_H

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

/**
 * @brief Validation methods for JSON documents
 *
 * There are many classes that need to ensure that JSON data received from an
 * external source contains the expected data. This class exists to fill that
 * need.
 *
 * The validation methods accept a reference to the expected type and return a
 * boolean indicating whether the provided data could be converted to that
 * type. Because a boolean is returned, multiple checks can be performed
 * within a single "if" statement joined with the logical and operator. For
 * example:
 *
 *     QJsonDocument doc = ...
 *     QJsonArray array;
 *     QString item1;
 *     qint64 item2;
 *
 *     if(JSON::isArray(doc, array) &&
 *             array.count() &&
 *             JSON::isObject(array.at(0), object) &&
 *             JSON::objectContains(object, "item1", item1) &&
 *             JSON::objectContains(object, "item2", item2) {
 *
 *         //...
 *     }
 *
 * One of the important things to note about this class is the fact that it
 * attempts to perform the conversion before indicating success or failure.
 * Therefore, the destination variable may be modified, even after failure.
 */
class Json
{
public:

    static bool isArray(const QJsonDocument &document, QJsonArray &array);
    static bool isObject(const QJsonDocument &document, QJsonObject &object);
    static bool isObject(const QJsonValue &value, QJsonObject &object);

    static bool objectContains(const QJsonObject &object, const QString &key, bool &value);
    static bool objectContains(const QJsonObject &object, const QString &key, qint32 &value);
    static bool objectContains(const QJsonObject &object, const QString &key, qint64 &value);
    static bool objectContains(const QJsonObject &object, const QString &key, QString &value);
};

#endif // NS_JSON_H
