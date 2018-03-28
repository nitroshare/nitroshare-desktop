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

#ifndef COMMON_TESTSERVER_H
#define COMMON_TESTSERVER_H

#include <QList>

#include <qmdnsengine/abstractserver.h>
#include <qmdnsengine/cache.h>
#include <qmdnsengine/message.h>

/**
 * @brief Test server that stores sent messages and enables manual message delivery
 */
class TestServer : public QMdnsEngine::AbstractServer
{
    Q_OBJECT

public:

    virtual void sendMessage(const QMdnsEngine::Message &message);
    virtual void sendMessageToAll(const QMdnsEngine::Message &message);

    void deliverMessage(const QMdnsEngine::Message &message);

    QList<QMdnsEngine::Message> receivedMessages() const;
    void clearReceivedMessages();

    const QMdnsEngine::Cache *cache() const;

private:

    void saveMessage(const QMdnsEngine::Message &message);

    QList<QMdnsEngine::Message> mMessages;
    QMdnsEngine::Cache mCache;
};

#endif // COMMON_TESTSERVER_H
