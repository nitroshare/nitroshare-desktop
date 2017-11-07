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
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QVariantMap>

#include <nitroshare/apiutil.h>

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
 * @param action storage for the action
 * @param params storage for the action parameters
 * @param error storage for an error message
 * @return true if no errors occurred
 */
bool parseData(const QByteArray &data, QString &action, QVariantMap &params, QString &error)
{
    QJsonObject object = QJsonDocument::fromJson(data).object();
    action = object.value("name").toString();
    params = object.value("parameters").toObject().toVariantMap();
    if (action.isNull()) {
        error = QObject::tr("invalid request received");
        return false;
    }
    return true;
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
    Q_UNUSED(app)

    forever {

            // Read the next message from STDIN
            QByteArray data;
            if (!readData(data)) {
                break;
            }

            QString error;

            // Parse the data
            QString action;
            QVariantMap params;
            if (!parseData(data, action, params, error)) {
                writeError(error);
                continue;
            }

            // Send the request
            QVariant returnVal;
            if (ApiUtil::sendRequest(action, params, returnVal, &error)) {

                // Send a reply with the JSON
                QJsonObject object = QJsonObject{
                    { "return", QJsonValue::fromVariant(returnVal) }
                };
                writeData(QJsonDocument(object).toJson());

            } else {
                writeError(error);
            }
    }

    return 0;
}
