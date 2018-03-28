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

#ifndef QMDNSENGINE_BROWSER_P_H
#define QMDNSENGINE_BROWSER_P_H

#include <QByteArray>
#include <QMap>
#include <QObject>
#include <QSet>
#include <QTimer>

#include <qmdnsengine/service.h>

namespace QMdnsEngine
{

class AbstractServer;
class Browser;
class Cache;
class Message;
class Record;

class BrowserPrivate : public QObject
{
    Q_OBJECT

public:

    explicit BrowserPrivate(Browser *browser, AbstractServer *server, const QByteArray &type, Cache *existingCache);

    bool updateService(const QByteArray &fqName);

    AbstractServer *server;
    QByteArray type;

    Cache *cache;
    QSet<QByteArray> ptrTargets;
    QMap<QByteArray, Service> services;

    QTimer queryTimer;
    QTimer serviceTimer;

private Q_SLOTS:

    void onMessageReceived(const Message &message);
    void onShouldQuery(const Record &record);
    void onRecordExpired(const Record &record);

    void onQueryTimeout();
    void onServiceTimeout();

private:

    Browser *const q;
};

}

#endif // QMDNSENGINE_BROWSER_P_H
