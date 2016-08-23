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

#include <QHostAddress>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUuid>

#include <nitroshare/application.h>
#include <nitroshare/logger.h>
#include <nitroshare/settings.h>

#include "apiserver.h"

const QString LoggerTag = "api";

const QString ApiEnabled = "ApiEnabled";

QVariant ApiEnabledDefault() { return true; }

ApiServer::ApiServer(Application *application)
    : QHttpServer(&mHandler),
      mApplication(application),
      mToken(QUuid::createUuid().toString()),
      mHandler(application, mToken)
{
    // Trigger loading the initial settings
    onSettingsChanged({ ApiEnabled });
}

ApiServer::~ApiServer()
{
    stop();
}

void ApiServer::onSettingsChanged(const QStringList &keys)
{
    if (keys.contains(ApiEnabled)) {
        stop();
        if (mApplication->settings()->get(ApiEnabled, &ApiEnabledDefault).toBool()) {
            start();
        }
    }
}

void ApiServer::start()
{
    if (!listen(QHostAddress::LocalHost)) {
        emit mApplication->logger()->error(LoggerTag, tr("Unable to find a port for the local API"));
        return;
    }

    emit mApplication->logger()->info(LoggerTag, tr("Listening on port %1").arg(serverPort()));

    if (!mLocalFile.open()) {
        emit mApplication->logger()->error(LoggerTag, tr("Unable to open \"%1\"").arg(mLocalFile.fileName()));
        return;
    }

    QJsonObject object({
        { "port", serverPort() },
        { "token", mToken }
    });

    mLocalFile.write(QJsonDocument(object).toJson());
    mLocalFile.close();
}

void ApiServer::stop()
{
    mLocalFile.remove();
    close();
}
