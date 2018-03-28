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

#ifndef QHTTPENGINE_PROXYSOCKET_H
#define QHTTPENGINE_PROXYSOCKET_H

#include <QHostAddress>
#include <QObject>
#include <QTcpSocket>

#include <qhttpengine/socket.h>

/**
 * @brief HTTP socket for connecting to a proxy
 *
 * The proxy socket manages the two socket connections - one for downstream
 * (the client's connection to the server) and one for upstream (the server's
 * connection to the upstream proxy).
 */
class ProxySocket: public QObject
{
    Q_OBJECT

public:

    explicit ProxySocket(QHttpEngine::Socket *socket, const QString &path, const QHostAddress &address, quint16 port);

private Q_SLOTS:

    void onDownstreamReadyRead();
    void onDownstreamDisconnected();

    void onUpstreamConnected();
    void onUpstreamReadyRead();
    void onUpstreamError(QTcpSocket::SocketError socketError);

private:

    QString methodToString(QHttpEngine::Socket::Method method) const;

    QHttpEngine::Socket *mDownstreamSocket;
    QTcpSocket mUpstreamSocket;

    QString mPath;
    bool mHeadersParsed;
    bool mHeadersWritten;

    QByteArray mUpstreamRead;
    QByteArray mUpstreamWrite;
};

#endif // QHTTPENGINE_PROXYSOCKET_H
