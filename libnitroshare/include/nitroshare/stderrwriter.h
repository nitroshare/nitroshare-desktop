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

#ifndef LIBNITROSHARE_STDERRWRITER_H
#define LIBNITROSHARE_STDERRWRITER_H

#include <QObject>

#include <nitroshare/config.h>
#include <nitroshare/logger.h>

class NITROSHARE_EXPORT StderrWriterPrivate;

/**
 * @brief Writer for printing log messages to stderr
 */
class NITROSHARE_EXPORT StderrWriter : public QObject
{
    Q_OBJECT

public:

    /**
     * @brief Create a new writer
     * @param parent QObject
     */
    explicit StderrWriter(QObject *parent = nullptr);

public Q_SLOTS:

    /**
     * @brief Write an informational message
     * @param messageType type of message
     * @param tag classifier for the message
     * @param message body of the message
     */
    void writeMessage(Logger::MessageType messageType, const QString &tag, const QString &message);

private:

    StderrWriterPrivate *const d;
};

#endif // LIBNITROSHARE_STDERRWRITER_H
