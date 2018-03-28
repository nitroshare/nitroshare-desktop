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

#ifndef QMDNSENGINE_ABSTRACTSERVER_H
#define QMDNSENGINE_ABSTRACTSERVER_H

#include <QObject>

#include "qmdnsengine_export.h"

namespace QMdnsEngine
{

class Message;

/**
 * @brief Base class for sending and receiving DNS messages
 *
 * Many of the other classes in this library require the ability to send and
 * receive DNS messages. By having them use this base class, they become far
 * easier to test. Any class derived from this one that implements the pure
 * virtual methods can be used for sending and receiving DNS messages.
 */
class QMDNSENGINE_EXPORT AbstractServer : public QObject
{
    Q_OBJECT

public:

    /**
     * @brief Abstract constructor
     */
    explicit AbstractServer(QObject *parent = 0);

    /**
     * @brief Send a message to its provided destination
     *
     * The message should be sent over the IP protocol specified in the
     * message and to the target address and port specified in the message.
     */
    virtual void sendMessage(const Message &message) = 0;

    /**
     * @brief Send a message to the multicast address on each interface
     *
     * The message should be sent over both IPv4 and IPv6 on all interfaces.
     */
    virtual void sendMessageToAll(const Message &message) = 0;

Q_SIGNALS:

    /**
     * @brief Indicate that a DNS message was received
     * @param message newly received message
     */
    void messageReceived(const Message &message);

    /**
     * @brief Indicate that an error has occurred
     * @param message brief description of the error
     */
    void error(const QString &message);
};

}

#endif // QMDNSENGINE_ABSTRACTSERVER_H
