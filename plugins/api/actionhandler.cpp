/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Nathan Osman
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

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QVariant>
#include <QVariantMap>

#include <nitroshare/action.h>
#include <nitroshare/actionregistry.h>
#include <nitroshare/application.h>

#include <qhttpengine/socket.h>

#include "actionhandler.h"

ActionHandler::ActionHandler(Application *application)
    : mApplication(application)
{
}

void ActionHandler::process(QHttpEngine::Socket *socket, const QString &path)
{
    // Ensure only POST requests are received
    if (socket->method() != QHttpEngine::Socket::POST) {
        socket->writeError(QHttpEngine::Socket::MethodNotAllowed);
        return;
    }

    // As soon as the read channel is finished, invoke the action
    connect(socket, &QHttpEngine::Socket::readChannelFinished, [this, socket, path]() {

        // Attempt to find the action with the specified name
        Action *action = mApplication->actionRegistry()->find(path);
        if (!action) {
            socket->writeError(QHttpEngine::Socket::NotFound);
            return;
        }

        // Convert the parameters from JSON to a QVariantMap
        QVariantMap params;
        QJsonDocument document;
        if (!socket->readJson(document)) {
            return;
        }
        params = document.object().toVariantMap();

        // Invoke the action, convert the response to JSON, and write it out
        socket->writeJson(QJsonDocument(QJsonObject{
            { "return", QJsonValue::fromVariant(action->invoke(params)) }
        }));
    });
}
