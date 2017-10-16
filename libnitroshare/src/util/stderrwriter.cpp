/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Nathan Osman
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

#include <iostream>

#include <QMap>

#include <nitroshare/stderrwriter.h>

#include "stderrwriter_p.h"

QMap<Logger::MessageType, QString> MessageTypeMap = {
    { Logger::Debug, "D" },
    { Logger::Info, "I" },
    { Logger::Warning, "W" },
    { Logger::Error, "E" }
};

StderrWriterPrivate::StderrWriterPrivate(QObject *parent)
    : QObject(parent)
{
}

StderrWriter::StderrWriter(QObject *parent)
    : QObject(parent),
      d(new StderrWriterPrivate(this))
{
}

void StderrWriter::writeMessage(Logger::MessageType messageType, const QString &tag, const QString &message)
{
    QString body = QString("[%1:%2] %3")
        .arg(MessageTypeMap.value(messageType))
        .arg(tag)
        .arg(message);
    std::cerr << body.toUtf8().constData() << std::endl;
}
