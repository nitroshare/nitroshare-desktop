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

#include <QVariant>

#include <nitroshare/application.h>
#include <nitroshare/logger.h>
#include <nitroshare/message.h>
#include <nitroshare/settings.h>

#include "apiserver.h"

const QString LoggerTag = "api";

// True to enable the HTTP API
const QString ApiEnabled = "ApiEnabled";
const QVariant ApiEnabledDefault = true;

ApiServer::ApiServer(Application *application)
    : mApplication(application),
      mApi(application),
      mServer(&mHandler)
{
    // Set up the handler
    mHandler.addMiddleware(&mAuth);
    mHandler.registerMethod("version", &mApi, &Api::version);
    mHandler.registerMethod("sendItems", &mApi, &Api::sendItems);

    // Add the setting for enabling the API and watch for it changing
    mApplication->settings()->addSetting(ApiEnabled, {{Settings::DefaultKey, ApiEnabledDefault}});
    connect(mApplication->settings(), &Settings::settingsChanged, this, &ApiServer::onSettingsChanged);

    // Trigger the initial settings
    onSettingsChanged({ ApiEnabled });
}

void ApiServer::start()
{
    // Bind to a local port
    if (!mServer.listen(QHostAddress::LocalHost)) {
        mApplication->logger()->log(new Message(
            Message::Error,
            LoggerTag,
            "unable to find a port for the local API"
        ));
        return;
    }

    // Log the port that we have bound to
    mApplication->logger()->log(new Message(
        Message::Info,
        LoggerTag,
        QString("listening on port %1").arg(mServer.serverPort())
    ));

    // Set the port in the local file
    mAuth.setData({{ "port", mServer.serverPort() }});
}

void ApiServer::stop()
{
    mServer.close();
}

void ApiServer::onSettingsChanged(const QStringList &keys)
{
    if (keys.contains(ApiEnabled)) {
        stop();
        if (mApplication->settings()->value(ApiEnabled).toBool()) {
            start();
        }
    }
}
