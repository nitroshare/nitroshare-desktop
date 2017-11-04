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

#ifndef LANTRANSPORTSERVER_H
#define LANTRANSPORTSERVER_H

#include "config.h"

#include <QStringList>

#ifdef ENABLE_TLS
#  include <QSslCertificate>
#  include <QSslConfiguration>
#  include <QSslKey>
#endif

#include <nitroshare/category.h>
#include <nitroshare/setting.h>
#include <nitroshare/transportserver.h>

#include "server.h"

class Application;

class LanTransportServer : public TransportServer
{
    Q_OBJECT

public:

    explicit LanTransportServer(Application *application);
    virtual ~LanTransportServer();

    virtual QString name() const;
    virtual Transport *createTransport(Device *device);

private slots:

    void onNewSocketDescriptor(qintptr socketDescriptor);
    void onSettingsChanged(const QStringList &keys);

private:

#ifdef ENABLE_TLS
    QSslCertificate loadCert(const QString &filename) const;
    QSslKey loadKey(const QString &filename, const QString &passphrase) const;
#endif

    Application *mApplication;

    Server mServer;

#ifdef ENABLE_TLS
    QSslConfiguration mSslConf;
#endif

    Category mTransferCategory;
    Setting mTransferPort;
#ifdef ENABLE_TLS
    Setting mTlsEnabled;
    Setting mTlsCaCertificate;
    Setting mTlsCertificate;
    Setting mTlsPrivateKey;
    Setting mTlsPrivateKeyPassphrase;
#endif
};

#endif // LANTRANSPORTSERVER_H
