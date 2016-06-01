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

#include <QHostAddress>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUuid>

#include "apiserver.h"
#include "apiserver_p.h"

ApiServerPrivate::ApiServerPrivate(ApiServer *apiServer)
    : QObject(apiServer),
      q(apiServer),
      token(QUuid::createUuid().toString()),
      handler(token),
      server(&handler)
{
    connect(&handler, &ApiHandler::bundleCreated, q, &ApiServer::bundleCreated, Qt::QueuedConnection);
    connect(Settings::instance(), &Settings::settingsChanged, this, &ApiServerPrivate::onSettingsChanged);
}

ApiServerPrivate::~ApiServerPrivate()
{
    stop();
}

void ApiServerPrivate::onSettingsChanged(const QList<Settings::Key> &keys)
{
    Settings *settings = Settings::instance();

    if (keys.empty() || keys.contains(Settings::Key::LocalAPI)) {
        stop();
        if (settings->get(Settings::Key::LocalAPI).toBool()) {
            start();
        }
    }
}

void ApiServerPrivate::stop()
{
    localFile.remove();
    server.close();
}

void ApiServerPrivate::start()
{
    if (!server.listen(QHostAddress::LocalHost)) {
        emit q->error(tr("Unable to find an available port for the local API"));
        return;
    }

    if (!localFile.open()) {
        emit q->error(tr("Unable to open %1").arg(localFile.fileName()));
        return;
    }

    QJsonObject object(QJsonObject::fromVariantMap({
        { "port", server.serverPort() },
        { "token", token }
    }));

    localFile.write(QJsonDocument(object).toJson());
    localFile.close();
}

ApiServer::ApiServer(QObject *parent)
    : QObject(parent),
      d(new ApiServerPrivate(this))
{
}

void ApiServer::start()
{
    d->onSettingsChanged();
}
