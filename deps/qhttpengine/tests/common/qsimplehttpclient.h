/*
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

#ifndef QHTTPENGINE_QSIMPLEHTTPCLIENT_H
#define QHTTPENGINE_QSIMPLEHTTPCLIENT_H

#include <QHostAddress>
#include <QObject>
#include <QTcpSocket>

#include <qhttpengine/parser.h>
#include <qhttpengine/socket.h>

/**
 * @brief Simple HTTP client for testing purposes
 *
 * This class emulates an extremely simple HTTP client for testing purposes.
 * Once a connection is established, headers and data can be sent and response
 * data is captured for later comparison.
 */
class QSimpleHttpClient : public QObject
{
    Q_OBJECT

public:

    QSimpleHttpClient(QTcpSocket *socket);

    void sendHeaders(const QByteArray &method, const QByteArray &path, const QHttpEngine::Socket::HeaderMap &headers = QHttpEngine::Socket::HeaderMap());
    void sendData(const QByteArray &data);

    int statusCode() const {
        return mStatusCode;
    }

    QByteArray statusReason() const {
        return mStatusReason;
    }

    QHttpEngine::Socket::HeaderMap headers() const {
        return mHeaders;
    }

    QByteArray data() const {
        return mData;
    }

    bool isDataReceived() const;

private Q_SLOTS:

    void onReadyRead();

private:

    QTcpSocket *mSocket;

    QByteArray mBuffer;
    bool mHeadersParsed;

    int mStatusCode;
    QByteArray mStatusReason;
    QHttpEngine::Socket::HeaderMap mHeaders;
    QByteArray mData;
};

#endif // QHTTPENGINE_QSIMPLEHTTPCLIENT_H
