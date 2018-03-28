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

#include <QPair>
#include <QUrl>
#include <QUrlQuery>

#include <qhttpengine/parser.h>

using namespace QHttpEngine;

void Parser::split(const QByteArray &data, const QByteArray &delim, int maxSplit, QByteArrayList &parts)
{
    int index = 0;

    for (int i = 0; !maxSplit || i < maxSplit; ++i) {
        int nextIndex = data.indexOf(delim, index);

        if (nextIndex == -1) {
            break;
        }

        parts.append(data.mid(index, nextIndex - index));
        index = nextIndex + delim.length();
    }

    // Append whatever remains to the list
    parts.append(data.mid(index));
}

bool Parser::parsePath(const QByteArray &rawPath, QString &path, Socket::QueryStringMap &queryString)
{
    QUrl url(rawPath);
    if (!url.isValid()) {
        return false;
    }

    path = url.path();
    QPair<QString, QString> pair;
    foreach (pair, QUrlQuery(url.query()).queryItems()) {
        queryString.insert(pair.first, pair.second);
    }

    return true;
}

bool Parser::parseHeaderList(const QList<QByteArray> &lines, Socket::HeaderMap &headers)
{
    foreach (const QByteArray &line, lines) {

        QList<QByteArray> parts;
        split(line, ":", 1, parts);

        // Ensure that the delimiter (":") was encountered at least once
        if (parts.count() != 2) {
            return false;
        }

        // Trim excess whitespace and add the header to the list
        headers.insert(parts[0].trimmed(), parts[1].trimmed());
    }

    return true;
}

bool Parser::parseHeaders(const QByteArray &data, QList<QByteArray> &parts, Socket::HeaderMap &headers)
{
    // Split the data into individual lines
    QList<QByteArray> lines;
    split(data, "\r\n", 0, lines);

    // Split the first line into a maximum of three parts
    split(lines.takeFirst(), " ", 2, parts);
    if (parts.count() != 3) {
        return false;
    }

    return parseHeaderList(lines, headers);
}

bool Parser::parseRequestHeaders(const QByteArray &data, Socket::Method &method, QByteArray &path, Socket::HeaderMap &headers)
{
    QList<QByteArray> parts;
    if (!parseHeaders(data, parts, headers)) {
        return false;
    }

    // Only HTTP/1.x versions are supported for now
    if (parts[2] != "HTTP/1.0" && parts[2] != "HTTP/1.1") {
        return false;
    }

    if (parts[0] == "OPTIONS") {
        method = Socket::OPTIONS;
    } else if (parts[0] == "GET") {
        method = Socket::GET;
    } else if (parts[0] == "HEAD") {
        method = Socket::HEAD;
    } else if (parts[0] == "POST") {
        method = Socket::POST;
    } else if (parts[0] == "PUT") {
        method = Socket::PUT;
    } else if (parts[0] == "DELETE") {
        method = Socket::DELETE;
    } else if (parts[0] == "TRACE") {
        method = Socket::TRACE;
    } else if (parts[0] == "CONNECT") {
        method = Socket::CONNECT;
    } else {
        return false;
    }

    path = parts[1];

    return true;
}

bool Parser::parseResponseHeaders(const QByteArray &data, int &statusCode, QByteArray &statusReason, Socket::HeaderMap &headers)
{
    QList<QByteArray> parts;
    if (!parseHeaders(data, parts, headers)) {
        return false;
    }

    statusCode = parts[1].toInt();
    statusReason = parts[2];

    // Ensure a valid status code
    return statusCode >= 100 && statusCode <= 599;
}
