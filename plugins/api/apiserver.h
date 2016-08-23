/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Nathan Osman
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

#ifndef APISERVER_H
#define APISERVER_H

#include <QHttpEngine/QHttpServer>
#include <QHttpEngine/QLocalFile>

#include "apihandler.h"

class Application;

/**
 * @brief Expose application functionality through HTTP
 *
 * This plugin exposes an HTTP server available to local processes that can be
 * used to control the application while it is running.
 */
class ApiServer : public QHttpServer
{
    Q_OBJECT

public:

    explicit ApiServer(Application *application);
    virtual ~ApiServer();

private slots:

    void onSettingsChanged(const QStringList &keys);

private:

    void start();
    void stop();

    Application *const mApplication;

    const QString mToken;
    QLocalFile mLocalFile;
    ApiHandler mHandler;
};

#endif // APISERVER_H
