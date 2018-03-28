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

#if !defined(QT_NO_SSL)
#  include <QSslSocket>
#endif

#include <qhttpengine/handler.h>
#include <qhttpengine/socket.h>

#include "server_p.h"

using namespace QHttpEngine;

ServerPrivate::ServerPrivate(Server *httpServer)
    : QObject(httpServer),
      q(httpServer),
      handler(0)
{
}

void ServerPrivate::process(QTcpSocket *socket)
{
    Socket *httpSocket = new Socket(socket, this);

    // Wait until the socket finishes reading the HTTP headers before routing
    connect(httpSocket, &Socket::headersParsed, [this, httpSocket]() {
        if (handler) {
            handler->route(httpSocket, QString(httpSocket->path().mid(1)));
        } else {
            httpSocket->writeError(Socket::InternalServerError);
        }
    });
}

Server::Server(QObject *parent)
    : QTcpServer(parent),
      d(new ServerPrivate(this))
{
}

Server::Server(Handler *handler, QObject *parent)
    : QTcpServer(parent),
      d(new ServerPrivate(this))
{
    setHandler(handler);
}

void Server::setHandler(Handler *handler)
{
    d->handler = handler;
}

#if !defined(QT_NO_SSL)
void Server::setSslConfiguration(const QSslConfiguration &configuration)
{
    d->configuration = configuration;
}
#endif

void Server::incomingConnection(qintptr socketDescriptor)
{
#if !defined(QT_NO_SSL)
    if (!d->configuration.isNull()) {

        // Initialize the socket with the SSL configuration
        QSslSocket *socket = new QSslSocket(this);

        // Wait until encryption is complete before processing the socket
        connect(socket, &QSslSocket::encrypted, [this, socket]() {
            d->process(socket);
        });

        // If an error occurs, delete the socket
        connect(socket, static_cast<void(QAbstractSocket::*)(QAbstractSocket::SocketError)>(&QAbstractSocket::error),
            socket, &QSslSocket::deleteLater);

        socket->setSocketDescriptor(socketDescriptor);
        socket->setSslConfiguration(d->configuration);
        socket->startServerEncryption();

    } else {
#endif

        QTcpSocket *socket = new QTcpSocket(this);
        socket->setSocketDescriptor(socketDescriptor);

        // Process the socket immediately
        d->process(socket);

#if !defined(QT_NO_SSL)
    }
#endif
}
