/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Nathan Osman
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
 **/

#ifndef NS_APISERVERPRIVATE_H
#define NS_APISERVERPRIVATE_H

#include "../settings/settings.h"
#include "apihandler.h"
#include "apiserver.h"

#if QHTTPENGINE_VERSION_MAJOR < 1
#  include <QHttpEngine/QLocalFile>
#  include <QHttpEngine/QHttpServer>
#else
#  include <qhttpengine/localauthmiddleware.h>
#  include <qhttpengine/server.h>
#endif

class ApiServerPrivate : public QObject
{
    Q_OBJECT

public:

    explicit ApiServerPrivate(ApiServer *apiServer);
    virtual ~ApiServerPrivate();

    const QString token;
    ApiHandler handler;

#if QHTTPENGINE_VERSION_MAJOR < 1
    QHttpServer server;
    QLocalFile localFile;
#else
    QHttpEngine::Server server;
    QHttpEngine::LocalAuthMiddleware middleware;
#endif

public Q_SLOTS:

    void onSettingsChanged(const QList<Settings::Key> &keys = {});

private:

    void stop();
    void start();

    ApiServer *const q;
};

#endif // NS_APISERVERPRIVATE_H
