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

#include <cstring>

#include <QJsonDocument>
#include <QJsonParseError>
#include <QTcpSocket>

#include <qhttpengine/parser.h>

#include "socket_p.h"

using namespace QHttpEngine;

// Predefined error response requires a simple HTML template to be returned to
// the client describing the error condition
const QString ErrorTemplate =
        "<!DOCTYPE html>"
        "<html>"
          "<head>"
            "<meta charset=\"utf-8\">"
            "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
            "<title>%1 %2</title>"
          "</head>"
          "<body>"
            "<h1>%1 %2</h1>"
            "<p>"
              "An error has occurred while trying to display the requested resource. "
              "Please contact the website owner if this error persists."
            "</p>"
            "<hr>"
            "<p><em>QHttpEngine %3</em></p>"
          "</body>"
        "</html>";

SocketPrivate::SocketPrivate(Socket *httpSocket, QTcpSocket *tcpSocket)
    : QObject(httpSocket),
      q(httpSocket),
      socket(tcpSocket),
      readState(ReadHeaders),
      requestDataRead(0),
      requestDataTotal(-1),
      writeState(WriteNone),
      responseStatusCode(200),
      responseStatusReason(statusReason(200))
{
    socket->setParent(this);

    connect(socket, &QTcpSocket::readyRead, this, &SocketPrivate::onReadyRead);
    connect(socket, &QTcpSocket::bytesWritten, this, &SocketPrivate::onBytesWritten);
    connect(socket, &QTcpSocket::readChannelFinished, this, &SocketPrivate::onReadChannelFinished);
    connect(socket, &QTcpSocket::disconnected, q, &Socket::disconnected);

    // Process anything already received by the socket
    onReadyRead();
}

QByteArray SocketPrivate::statusReason(int statusCode) const
{
    switch (statusCode) {
    case Socket::OK: return "OK";
    case Socket::Created: return "CREATED";
    case Socket::Accepted: return "ACCEPTED";
    case Socket::PartialContent: return "PARTIAL CONTENT";
    case Socket::MovedPermanently: return "MOVED PERMANENTLY";
    case Socket::Found: return "FOUND";
    case Socket::BadRequest: return "BAD REQUEST";
    case Socket::Unauthorized: return "UNAUTHORIZED";
    case Socket::Forbidden: return "FORBIDDEN";
    case Socket::NotFound: return "NOT FOUND";
    case Socket::MethodNotAllowed: return "METHOD NOT ALLOWED";
    case Socket::Conflict: return "CONFLICT";
    case Socket::BadGateway: return "BAD GATEWAY";
    case Socket::ServiceUnavailable: return "SERVICE UNAVAILABLE";
    case Socket::InternalServerError: return "INTERNAL SERVER ERROR";
    case Socket::HttpVersionNotSupported: return "HTTP VERSION NOT SUPPORTED";
    default: return "UNKNOWN ERROR";
    }
}

void SocketPrivate::onReadyRead()
{
    // Append all of the new data to the read buffer
    readBuffer.append(socket->readAll());

    // If reading headers, return if they could not be read (yet)
    if (readState == ReadHeaders && !readHeaders()) {
        return;
    }

    // Read data if in that state, otherwise discard
    switch (readState) {
    case ReadData:
        readData();
        break;
    case ReadFinished:
        readBuffer.clear();
        break;
    }
}

void SocketPrivate::onBytesWritten(qint64 bytes)
{
    // Check to see if all of the response header was written
    if (writeState == WriteHeaders) {
        if (responseHeaderRemaining - bytes > 0) {
            responseHeaderRemaining -= bytes;
        } else {
            writeState = WriteData;
            bytes -= responseHeaderRemaining;
        }
    }

    // Only emit bytesWritten() for data after the headers
    if (writeState == WriteData) {
        Q_EMIT q->bytesWritten(bytes);
    }
}

void SocketPrivate::onReadChannelFinished()
{
    if (requestDataTotal == -1) {
        emit q->readChannelFinished();
    }
}

bool SocketPrivate::readHeaders()
{
    // Check for the double CRLF that signals the end of the headers and
    // if it is not found, wait until the next time readyRead is emitted
    int index = readBuffer.indexOf("\r\n\r\n");
    if (index == -1) {
        return false;
    }

    // Attempt to parse the headers and if a problem is encountered, abort
    // the connection (so that no more data is read or written) and return
    if (!Parser::parseRequestHeaders(readBuffer.left(index), requestMethod, requestRawPath, requestHeaders) ||
            !Parser::parsePath(requestRawPath, requestPath, requestQueryString)) {
        q->writeError(Socket::BadRequest);
        return false;
    }

    // Remove the headers from the buffer
    readBuffer.remove(0, index + 4);
    readState = ReadData;

    // If the content-length header is present, use it to determine
    // how much data to expect from the socket - not all requests
    // use this header - WebSocket requests, for example, do not
    if (requestHeaders.contains("Content-Length")) {
        requestDataTotal = requestHeaders.value("Content-Length").toLongLong();
    }

    // Indicate that the headers have been parsed
    Q_EMIT q->headersParsed();

    return true;
}

void SocketPrivate::readData()
{
    // Emit the readyRead() signal if any data is available in the buffer
    if (readBuffer.size()) {
        Q_EMIT q->readyRead();
    }

    // Check to see if the specified amount of data has been read from the
    // socket, if so, emit the readChannelFinished() signal
    if (requestDataTotal != -1 &&
            requestDataRead + readBuffer.size() >= requestDataTotal) {
        readState = ReadFinished;
        Q_EMIT q->readChannelFinished();
    }
}

Socket::Socket(QTcpSocket *socket, QObject *parent)
    : QIODevice(parent),
      d(new SocketPrivate(this, socket))
{
    // The device is initially open for both reading and writing
    setOpenMode(QIODevice::ReadWrite);
}

qint64 Socket::bytesAvailable() const
{
    if (d->readState > SocketPrivate::ReadHeaders) {
        return d->readBuffer.size() + QIODevice::bytesAvailable();
    } else {
        return 0;
    }
}

bool Socket::isSequential() const
{
    return true;
}

void Socket::close()
{
    // Invoke the parent method
    QIODevice::close();

    d->readState = SocketPrivate::ReadFinished;
    d->writeState = SocketPrivate::WriteFinished;

    connect(d->socket, &QTcpSocket::disconnected, this, &Socket::deleteLater);
    d->socket->close();
}

QHostAddress Socket::peerAddress() const
{
    return d->socket->peerAddress();
}

bool Socket::isHeadersParsed() const
{
    return d->readState > SocketPrivate::ReadHeaders;
}

Socket::Method Socket::method() const
{
    return d->requestMethod;
}

QByteArray Socket::rawPath() const
{
    return d->requestRawPath;
}

QString Socket::path() const
{
    return d->requestPath;
}

Socket::QueryStringMap Socket::queryString() const
{
    return d->requestQueryString;
}

Socket::HeaderMap Socket::headers() const
{
    return d->requestHeaders;
}

qint64 Socket::contentLength() const
{
    return d->requestDataTotal;
}

bool Socket::readJson(QJsonDocument &document)
{
    QJsonParseError error;
    document = QJsonDocument::fromJson(readAll(), &error);

    if (error.error != QJsonParseError::NoError) {
        writeError(Socket::BadRequest);
        return false;
    }

    return true;
}

void Socket::setStatusCode(int statusCode, const QByteArray &statusReason)
{
    d->responseStatusCode = statusCode;
    d->responseStatusReason = statusReason.isNull() ? d->statusReason(statusCode) : statusReason;
}

void Socket::setHeader(const QByteArray &name, const QByteArray &value, bool replace)
{
    if (replace || d->responseHeaders.count(name)) {
        d->responseHeaders.replace(name, value);
    } else {
        d->responseHeaders.replace(name, d->responseHeaders.value(name) + ", " + value);
    }
}

void Socket::setHeaders(const HeaderMap &headers)
{
    d->responseHeaders = headers;
}

void Socket::writeHeaders()
{
    // Use a QByteArray for building the header so that we can later determine
    // exactly how many bytes were written
    QByteArray header;

    // Append the status line
    header.append("HTTP/1.0 ");
    header.append(QByteArray::number(d->responseStatusCode) + " " + d->responseStatusReason);
    header.append("\r\n");

    // Append each of the headers followed by a CRLF
    for (auto i = d->responseHeaders.constBegin(); i != d->responseHeaders.constEnd(); ++i) {
        header.append(i.key());
        header.append(": ");
        header.append(d->responseHeaders.values(i.key()).join(", "));
        header.append("\r\n");
    }

    // Append an extra CRLF
    header.append("\r\n");

    d->writeState = SocketPrivate::WriteHeaders;
    d->responseHeaderRemaining = header.length();

    // Write the header
    d->socket->write(header);
}

void Socket::writeRedirect(const QByteArray &path, bool permanent)
{
    setStatusCode(permanent ? MovedPermanently : Found);
    setHeader("Location", path);
    writeHeaders();
    close();
}

void Socket::writeError(int statusCode, const QByteArray &statusReason)
{
    setStatusCode(statusCode, statusReason);

    // Build the template that will be sent to the client
    QByteArray data = ErrorTemplate
            .arg(d->responseStatusCode)
            .arg(d->responseStatusReason.constData())
            .arg(QHTTPENGINE_VERSION)
            .toUtf8();

    setHeader("Content-Length", QByteArray::number(data.length()));
    setHeader("Content-Type", "text/html");

    writeHeaders();
    write(data);
    close();
}

void Socket::writeJson(const QJsonDocument &document, int statusCode)
{
    QByteArray data = document.toJson();
    setStatusCode(statusCode);
    setHeader("Content-Length", QByteArray::number(data.length()));
    setHeader("Content-Type", "application/json");
    write(data);
    close();
}

qint64 Socket::readData(char *data, qint64 maxlen)
{
    // Ensure the connection is in the correct state for reading data
    if (d->readState == SocketPrivate::ReadHeaders) {
        return 0;
    }

    // Ensure that no more than the requested amount or the size of the buffer is read
    qint64 size = qMin(static_cast<qint64>(d->readBuffer.size()), maxlen);
    memcpy(data, d->readBuffer.constData(), size);

    // Remove the amount that was read from the buffer
    d->readBuffer.remove(0, size);
    d->requestDataRead += size;

    return size;
}

qint64 Socket::writeData(const char *data, qint64 len)
{
    // If the response headers have not yet been written, they must
    // immediately be written before the data can be
    if (d->writeState == SocketPrivate::WriteNone) {
        writeHeaders();
    }

    return d->socket->write(data, len);
}
