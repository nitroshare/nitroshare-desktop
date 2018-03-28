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

#ifndef QMDNSENGINE_SERVER_H
#define QMDNSENGINE_SERVER_H

#include <qmdnsengine/abstractserver.h>

#include "qmdnsengine_export.h"

namespace QMdnsEngine
{

class Message;

class QMDNSENGINE_EXPORT ServerPrivate;

/**
 * @brief mDNS server
 *
 * This class provides an implementation of
 * [AbstractServer](@ref QMdnsEngine::AbstractServer) that uses all available
 * local network adapters to send and receive mDNS messages.
 */
class QMDNSENGINE_EXPORT Server : public AbstractServer
{
    Q_OBJECT

public:

    /**
     * @brief Create a new server
     */
    explicit Server(QObject *parent = 0);

    /**
     * @brief Implementation of AbstractServer::sendMessage()
     */
    virtual void sendMessage(const Message &message);

    /**
     * @brief Implementation of AbstractServer::sendMessageToAll()
     */
    virtual void sendMessageToAll(const Message &message);

private:

    ServerPrivate *const d;
};

}

#endif // QMDNSENGINE_SERVER_H
