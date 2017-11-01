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

#include <cstdio>

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>

/**
 * @brief Read the next message from STDIN
 * @param data storage for the message content
 * @return true if a message was read; false if EOF
 */
bool readData(QByteArray &data)
{
    qint32 dataSize;
    if (fread(&dataSize, sizeof(dataSize), 1, stdin) != 1) {
        return false;
    }
    data.resize(dataSize);
    if (fread(data.data(), 1, dataSize, stdin) != dataSize) {
        return false;
    }
    return true;
}

/**
 * @brief Parse data in a message
 * @param data message content
 * @param name storage for the action name
 * @param parameters storage for the action parameters
 * @param error storage for an error message
 * @return true if no errors occurred
 */
bool parseData(const QByteArray &data, QString &name, QByteArray &parameters, QString &error)
{
    QJsonObject object = QJsonDocument::fromJson(data).object();
    name = object.value("name").toString();
    QJsonObject parametersObject = object.value("parameters").toObject();
    parameters = QJsonDocument(parametersObject).toJson(QJsonDocument::Compact);
    if (name.isNull()) {
        error = QObject::tr("invalid request received");
        return false;
    }
    return true;
}

/**
 * @brief Attempt to find NitroShare running under the same account
 * @param port storage for the local port to connect to
 * @param token storage for the auth token
 * @param error storage for an error message
 * @return true if NitroShare was found running
 */
bool findNitroShare(quint16 &port, QString &token, QString &error)
{
    QFile file(QDir::home().absoluteFilePath(".nitroshare-ui"));
    if (!file.open(QIODevice::ReadOnly)) {
        error = QObject::tr("NitroShare does not appear to be running");
        return false;
    }
    QJsonObject object = QJsonDocument::fromJson(file.readAll()).object();
    port = object.value("port").toInt();
    token = object.value("token").toString();
    if (!port || token.isNull()) {
        error = QObject::tr("unable to read NitroShare configuration");
        return false;
    }
    return true;
}

/**
 * @brief Create a request to the NitroShare client
 * @param port NitroShare port
 * @param name action name
 * @param token auth token
 * @return newly created request
 */
QNetworkRequest createRequest(quint16 port, const QString &name, const QString &token)
{
    QUrl url(QString("http://localhost:%1/%2").arg(port).arg(name));
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("X-Auth-Token", token.toUtf8());
    return request;
}

/**
 * @brief Write data to STDIN
 * @param data message content
 */
void writeData(const QByteArray &data)
{
    qint32 dataSize = data.size();
    fwrite(&dataSize, sizeof(dataSize), 1, stdout);
    fwrite(data.constData(), 1, dataSize, stdout);
    fflush(stdout);
}

/**
 * @brief Write an error to STDIN
 * @param message descriptive message
 */
void writeError(const QString &message)
{
    writeData(QJsonDocument(QJsonObject{
        { "error", message }
    }).toJson(QJsonDocument::Compact));
}

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    QNetworkAccessManager manager;

    forever {

        // Read the next message from STDIN
        QByteArray data;
        if (!readData(data)) {
            break;
        }

        QString error;

        // Parse the data
        QString name;
        QByteArray parameters;
        if (!parseData(data, name, parameters, error)) {
            writeError(error);
            continue;
        }

        // Find NitroShare (if it is running)
        quint16 port;
        QString token;
        if (!findNitroShare(port, token, error)) {
            writeError(error);
            continue;
        }

        // Send the request and wait for completion
        QNetworkReply *reply = manager.post(createRequest(port, name, token), parameters);

        // Process the request when it completes
        QObject::connect(reply, &QNetworkReply::finished, [reply]() {
            reply->deleteLater();
            if (reply->error() != QNetworkReply::NoError ||
                    reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() != 200) {
                writeError(QObject::tr("error communicating with NitroShare"));
                return;
            }
            QJsonParseError jsonError;
            QJsonDocument document = QJsonDocument::fromJson(reply->readAll(), &jsonError);
            if (jsonError.error != QJsonParseError::NoError) {
                writeError(QObject::tr("invalid response received"));
                return;
            }
            writeData(document.toJson(QJsonDocument::Compact));
        });

        // Quit the event loop when the reply is received
        QObject::connect(reply, &QNetworkReply::finished, &app, &QCoreApplication::quit);

        // Run the event loop
        app.exec();
    }

    return 0;
}
