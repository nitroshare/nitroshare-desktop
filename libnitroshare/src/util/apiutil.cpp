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

#include <QDir>
#include <QEventLoop>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QObject>

#include <nitroshare/apiutil.h>

bool findNitroShare(quint16 &port, QString &token, QString *error)
{
    // Open the file that contains the information
    QFile file(QDir::home().absoluteFilePath(".NitroShare"));
    if (!file.open(QIODevice::ReadOnly)) {
        if (error) {
            *error = file.errorString();
        }
        return false;
    }

    // Read the content as a JSON document
    QJsonParseError jsonError;
    QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &jsonError);
    file.close();
    if (jsonError.error != QJsonParseError::NoError) {
        if (error) {
            *error = jsonError.errorString();
        }
        return false;
    }

    // Verify and extract the information
    if (!document.isObject()) {
        if (error) {
            *error = QObject::tr("object expected");
        }
        return false;
    }
    QJsonObject object = document.object();
    if (!object.contains("port") || !object.contains("token")) {
        if (error) {
            *error = QObject::tr("\"port\" and \"token\" required");
        }
        return false;
    }

    // Fill in the values
    port = object.value("port").toInt();
    token = object.value("token").toString();

    return true;
}

bool ApiUtil::sendRequest(const QString &action,
                          const QVariantMap &params,
                          QVariant &returnVal,
                          QString *error)
{
    QNetworkAccessManager networkAccessManager;
    quint16 port;
    QString token;

    // Retrieve the information needed to connect to NitroShare
    if (!findNitroShare(port, token, error)) {
        return false;
    }

    // Prepare the request
    QNetworkRequest request(QUrl(
        QString("http://localhost:%1/api/%2").arg(port).arg(action)
    ));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("X-Auth-Token", token.toUtf8());

    // Send the request
    QByteArray data = QJsonDocument::fromVariant(params).toJson(QJsonDocument::Compact);
    QNetworkReply *reply = networkAccessManager.post(request, data);

    QEventLoop eventLoop;
    bool succeeded = false;

    // Process the request when it completes
    QObject::connect(reply, &QNetworkReply::finished, [&]() {

        // Ensure the reply is freed
        reply->deleteLater();

        // Fail if an error was returned
        if (reply->error() != QNetworkReply::NoError) {
            if (error) {
                *error = reply->errorString();
            }
            return;
        }

        // Attempt to parse the response as JSON
        QJsonParseError jsonError;
        QJsonDocument document = QJsonDocument::fromJson(reply->readAll(), &jsonError);
        if (jsonError.error != QJsonParseError::NoError) {
            if (error) {
                *error = jsonError.errorString();
            }
            return;
        }

        // Verify and extract the information
        if (!document.isObject()) {
            if (error) {
                *error = QObject::tr("object expected");
            }
            return;
        }
        QJsonObject object = document.object();
        if (!object.contains("return")) {
            if (error) {
                *error = QObject::tr("\"return\" required");
            }
            return;
        }

        // Fill in the return value
        returnVal = object.value("return").toVariant();
        succeeded = true;
    });

    // Quit the event loop when the request completes
    QObject::connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);

    // Run the event loop
    eventLoop.exec();

    return succeeded;
}
