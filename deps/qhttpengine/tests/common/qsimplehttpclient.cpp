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

#include "qsimplehttpclient.h"

QSimpleHttpClient::QSimpleHttpClient(QTcpSocket *socket)
    : mSocket(socket),
      mHeadersParsed(false),
      mStatusCode(0)
{
    connect(mSocket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));

    // Immediately trigger a read
    onReadyRead();
}

void QSimpleHttpClient::sendHeaders(const QByteArray &method, const QByteArray &path, const QHttpEngine::Socket::HeaderMap &headers)
{
    QByteArray data = method + " " + path + " HTTP/1.0\r\n";
    for (auto i = headers.constBegin(); i != headers.constEnd(); ++i) {
        data.append(i.key() + ": " + i.value() + "\r\n");
    }
    data.append("\r\n");

    mSocket->write(data);
}

void QSimpleHttpClient::sendData(const QByteArray &data)
{
    mSocket->write(data);
}

void QSimpleHttpClient::onReadyRead()
{
    if (mHeadersParsed) {
        mData.append(mSocket->readAll());
    } else {
        mBuffer.append(mSocket->readAll());

        // Parse the headers if the double CRLF sequence was found
        int index = mBuffer.indexOf("\r\n\r\n");
        if (index != -1) {
            QHttpEngine::Parser::parseResponseHeaders(mBuffer.left(index), mStatusCode, mStatusReason, mHeaders);

            mHeadersParsed = true;
            mData.append(mBuffer.mid(index + 4));
        }
    }
}

bool QSimpleHttpClient::isDataReceived() const
{
    return mHeaders.contains("Content-Length") &&
            mData.length() >= mHeaders.value("Content-Length").toInt();
}
