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

#ifndef LIBNITROSHARE_MESSAGE_H
#define LIBNITROSHARE_MESSAGE_H

#include <QDateTime>
#include <QObject>

#include <nitroshare/config.h>

class NITROSHARE_EXPORT MessagePrivate;

/**
 * @brief Log message
 *
 * Although it may seem like overkill, inheriting from QObject enables the
 * class to expose its enums and properties to moc.
 */
class NITROSHARE_EXPORT Message : public QObject
{
    Q_OBJECT
    Q_ENUMS(Type)
    Q_PROPERTY(Type type READ type)
    Q_PROPERTY(QString tag READ tag)
    Q_PROPERTY(QString body READ body)

public:

    /**
     * @brief Type of message
     */
    enum Type {
        Debug,
        Info,
        Warning,
        Error
    };

    /**
     * @brief Construct a new log message
     * @param type type of message
     * @param tag classifier for the message
     * @param body content of the message
     */
    Message(Type type, const QString &tag, const QString &body);

    /**
     * @brief Retrieve the date and time of the message
     */
    QDateTime dateTime() const;

    /**
     * @brief Retrieve message type
     */
    Type type() const;

    /**
     * @brief Retrieve message classifier
     */
    QString tag() const;

    /**
     * @brief Retrieve message content
     */
    QString body() const;

    /**
     * @brief Combine the message data into a single string
     */
    QString toString() const;

private:

    MessagePrivate *const d;
};

#endif // LIBNITROSHARE_MESSAGE_H
