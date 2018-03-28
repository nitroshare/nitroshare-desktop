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

#include <qhttpengine/parser.h>

#include "proxysocket.h"

using namespace QHttpEngine;

ProxySocket::ProxySocket(Socket *socket, const QString &path, const QHostAddress &address, quint16 port)
    : QObject(socket),
      mDownstreamSocket(socket),
      mPath(path),
      mHeadersParsed(false),
      mHeadersWritten(false)
{
    connect(mDownstreamSocket, &Socket::readyRead, this, &ProxySocket::onDownstreamReadyRead);
    connect(mDownstreamSocket, &Socket::disconnected, this, &ProxySocket::onDownstreamDisconnected);

    connect(&mUpstreamSocket, &QTcpSocket::connected, this, &ProxySocket::onUpstreamConnected);
    connect(&mUpstreamSocket, &QTcpSocket::readyRead, this, &ProxySocket::onUpstreamReadyRead);
    connect(
        &mUpstreamSocket,
        static_cast<void(QAbstractSocket::*)(QAbstractSocket::SocketError)>(&QAbstractSocket::error),
        this,
        &ProxySocket::onUpstreamError
    );

    mUpstreamSocket.connectToHost(address, port);
}

void ProxySocket::onDownstreamReadyRead()
{
    if (mHeadersWritten) {
        mUpstreamSocket.write(mDownstreamSocket->readAll());
    } else {
        mUpstreamWrite.append(mDownstreamSocket->readAll());
    }
}

void ProxySocket::onDownstreamDisconnected()
{
    mUpstreamSocket.disconnectFromHost();
}

void ProxySocket::onUpstreamConnected()
{
    // Write the status line using the stripped path from the handler
    mUpstreamSocket.write(
        QString("%1 /%2 HTTP/1.1\r\n")
            .arg(methodToString(mDownstreamSocket->method()))
            .arg(mPath)
            .toUtf8()
    );

    // Use the existing headers but insert proxy-related ones
    Socket::HeaderMap headers = mDownstreamSocket->headers();
    QByteArray peerIP = mDownstreamSocket->peerAddress().toString().toUtf8();
    QByteArray origFwd = headers.value("X-Forwarded-For");
    if (origFwd.isNull()) {
        headers.insert("X-Forwarded-For", peerIP);
    } else {
        headers.insert("X-Forwarded-For", origFwd + ", " + peerIP);
    }
    if (!headers.contains("X-Real-IP")) {
        headers.insert("X-Real-IP", peerIP);
    }

    // Write the headers to the socket with the terminating CRLF
    for (auto i = headers.constBegin(); i != headers.constEnd(); ++i) {
        mUpstreamSocket.write(i.key() + ": " + i.value() + "\r\n");
    }
    mUpstreamSocket.write("\r\n");
    mHeadersWritten = true;

    // If there is any data buffered for writing, write it
    if (mUpstreamWrite.size()) {
        mUpstreamSocket.write(mUpstreamWrite);
        mUpstreamWrite.clear();
    }
}

void ProxySocket::onUpstreamReadyRead()
{
    // If the headers have not yet been parsed, then check to see if the end
    // has been reached yet; if they have, just dump data

    if (!mHeadersParsed) {

        // Add to the buffer and check to see if the end was reached
        mUpstreamRead.append(mUpstreamSocket.readAll());
        int index = mUpstreamRead.indexOf("\r\n\r\n");
        if (index != -1) {

            // Parse the headers
            int statusCode;
            QByteArray statusReason;
            Socket::HeaderMap headers;
            if (!Parser::parseResponseHeaders(mUpstreamRead.left(index), statusCode, statusReason, headers)) {
                mDownstreamSocket->writeError(Socket::BadGateway);
                return;
            }

            // Dump the headers back downstream
            mDownstreamSocket->setStatusCode(statusCode, statusReason);
            mDownstreamSocket->setHeaders(headers);
            mDownstreamSocket->writeHeaders();
            mDownstreamSocket->write(mUpstreamRead.mid(index + 4));

            // Remember that headers were parsed and empty the buffer
            mHeadersParsed = true;
            mUpstreamRead.clear();
        }
    } else {
        mDownstreamSocket->write(mUpstreamSocket.readAll());
    }
}

void ProxySocket::onUpstreamError(QAbstractSocket::SocketError socketError)
{
    if (mHeadersParsed) {
        mDownstreamSocket->close();
    } else {
        mDownstreamSocket->writeError(Socket::BadGateway);
    }
}

QString ProxySocket::methodToString(Socket::Method method) const
{
    switch (method) {
    case Socket::OPTIONS: return "OPTIONS";
    case Socket::GET: return "GET";
    case Socket::HEAD: return "HEAD";
    case Socket::POST: return "POST";
    case Socket::PUT: return "PUT";
    case Socket::DELETE: return "DELETE";
    case Socket::TRACE: return "TRACE";
    case Socket::CONNECT: return "CONNECT";
    default: return QString();
    }
}
