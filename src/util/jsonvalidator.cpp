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

#include "jsonvalidator.h"

bool JsonValidator::isArray(const QJsonDocument &document, QJsonArray &array)
{
    array = document.array();
    return document.isArray();
}

bool JsonValidator::isObject(const QJsonDocument &document, QJsonObject &object)
{
    object = document.object();
    return document.isObject();
}

bool JsonValidator::isObject(const QJsonValue &value, QJsonObject &object)
{
    object = value.toObject();
    return value.isObject();
}

bool JsonValidator::objectContains(const QJsonObject &object, const QString &key, QString &value)
{
    value = object.value(key).toString();
    return object.contains(key);
}

bool JsonValidator::objectContains(const QJsonObject &object, const QString &key, qint32 &value)
{
    value = object.value(key).toString().toInt();
    return object.contains(key);
}

bool JsonValidator::objectContains(const QJsonObject &object, const QString &key, qint64 &value)
{
    value = object.value(key).toString().toLongLong();
    return object.contains(key);
}
