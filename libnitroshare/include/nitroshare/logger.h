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

#ifndef LIBNITROSHARE_LOGGER_H
#define LIBNITROSHARE_LOGGER_H

#include <QList>
#include <QObject>

#include <nitroshare/config.h>

class Message;

class NITROSHARE_EXPORT LoggerPrivate;

/**
 * @brief Manage status and error message from the application and its plugins
 *
 * This class models the fan-out messaging pattern. All messages are sent to
 * this class for dispatch, allowing plugins to react to the messages as they
 * are generated.
 */
class NITROSHARE_EXPORT Logger : public QObject
{
    Q_OBJECT

public:

    /**
     * @brief Create a new logger
     * @param parent QObject
     */
    explicit Logger(QObject *parent = nullptr);

    /**
     * @brief Retrieve all messages that have been logged
     * @return list of messages
     */
    QList<Message*> messages() const;

public Q_SLOTS:

    /**
     * @brief Log the specified message
     * @param message pointer to Message
     *
     * This class will assume ownership of the message.
     */
    void log(Message *message);

Q_SIGNALS:

    /**
     * @brief Indicate that a message was logged
     * @param message pointer to Message
     */
    void messageLogged(const Message *message);

private:

    LoggerPrivate *const d;
};

#endif // LIBNITROSHARE_LOGGER_H
