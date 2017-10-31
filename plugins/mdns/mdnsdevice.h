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

#ifndef MDNSDEVICE_H
#define MDNSDEVICE_H

#include <nitroshare/device.h>

#include <qmdnsengine/cache.h>
#include <qmdnsengine/resolver.h>
#include <qmdnsengine/server.h>
#include <qmdnsengine/service.h>

class MdnsDevice : public Device
{
    Q_OBJECT
    Q_PROPERTY(QStringList addresses READ addresses)
    Q_PROPERTY(quint16 port READ port)

public:

    explicit MdnsDevice(QMdnsEngine::Server *server,
                        QMdnsEngine::Cache *cache,
                        const QMdnsEngine::Service &service);

    virtual QString uuid() const;
    virtual QString name() const;

    QStringList addresses() const;
    quint16 port() const;

    void update(const QMdnsEngine::Service &service);

private slots:

    void onResolved(const QHostAddress &address);

private:

    QString mUuid;
    QString mName;
    QStringList mAddresses;
    quint16 mPort;

    QMdnsEngine::Resolver mResolver;
};

#endif // MDNSDEVICE_H
