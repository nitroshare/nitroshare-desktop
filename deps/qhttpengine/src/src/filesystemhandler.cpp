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

#include <QFile>
#include <QFileInfo>
#include <QFileInfoList>
#include <QUrl>

#include <qhttpengine/filesystemhandler.h>
#include <qhttpengine/qiodevicecopier.h>
#include <qhttpengine/range.h>
#include <qhttpengine/socket.h>

#include "filesystemhandler_p.h"

using namespace QHttpEngine;

// Template for listing directory contents
const QString ListTemplate =
        "<!DOCTYPE html>"
        "<html>"
          "<head>"
            "<meta charset=\"utf-8\">"
            "<title>%1</title>"
          "</head>"
          "<body>"
            "<h1>%1</h1>"
            "<p>Directory listing:</p>"
            "<ul>%2</ul>"
            "<hr>"
            "<p><em>QHttpEngine %3</em></p>"
          "</body>"
        "</html>";

FilesystemHandlerPrivate::FilesystemHandlerPrivate(FilesystemHandler *handler)
    : QObject(handler)
{
}

bool FilesystemHandlerPrivate::absolutePath(const QString &path, QString &absolutePath)
{
    // Resolve the path according to the document root
    absolutePath = documentRoot.absoluteFilePath(path);

    // Perhaps not the most efficient way of doing things, but one way to
    // determine if path is within the document root is to convert it to a
    // relative path and check to see if it begins with "../" (it shouldn't)
    return documentRoot.exists(absolutePath) && !documentRoot.relativeFilePath(path).startsWith("../");
}

QByteArray FilesystemHandlerPrivate::mimeType(const QString &absolutePath)
{
    // Query the MIME database based on the filename and its contents
    return database.mimeTypeForFile(absolutePath).name().toUtf8();
}

void FilesystemHandlerPrivate::processFile(Socket *socket, const QString &absolutePath)
{
    // Attempt to open the file for reading
    QFile *file = new QFile(absolutePath);
    if (!file->open(QIODevice::ReadOnly)) {
        delete file;

        socket->writeError(Socket::Forbidden);
        return;
    }

    // Create a QIODeviceCopier to copy the file contents to the socket
    QIODeviceCopier *copier = new QIODeviceCopier(file, socket);
    connect(copier, &QIODeviceCopier::finished, copier, &QIODeviceCopier::deleteLater);
    connect(copier, &QIODeviceCopier::finished, file, &QFile::deleteLater);
    connect(copier, &QIODeviceCopier::finished, [socket]() {
        socket->close();
    });

    // Stop the copier if the socket is disconnected
    connect(socket, &Socket::disconnected, copier, &QIODeviceCopier::stop);

    qint64 fileSize = file->size();

    // Checking for partial content request
    QByteArray rangeHeader = socket->headers().value("Range");
    Range range;

    if (!rangeHeader.isEmpty() && rangeHeader.startsWith("bytes=")) {
        // Skiping 'bytes=' - first 6 chars and spliting ranges by comma
        QList<QByteArray> rangeList = rangeHeader.mid(6).split(',');

        // Taking only first range, as multiple ranges require multipart
        // reply support
        range = Range(QString(rangeList.at(0)), fileSize);
    }

    // If range is valid, send partial content
    if (range.isValid()) {
        socket->setStatusCode(Socket::PartialContent);
        socket->setHeader("Content-Length", QByteArray::number(range.length()));
        socket->setHeader("Content-Range", QByteArray("bytes ") + range.contentRange().toLatin1());
        copier->setRange(range.from(), range.to());
    } else {
        // If range is invalid or if it is not a partial content request,
        // send full file
        socket->setHeader("Content-Length", QByteArray::number(fileSize));
    }

    // Set the mimetype and content length
    socket->setHeader("Content-Type", mimeType(absolutePath));
    socket->writeHeaders();

    // Start the copy
    copier->start();
}

void FilesystemHandlerPrivate::processDirectory(Socket *socket, const QString &path, const QString &absolutePath)
{
    // Add entries for each of the files
    QString listing;
    foreach (QFileInfo info, QDir(absolutePath).entryInfoList(QDir::NoFilter, QDir::Name | QDir::DirsFirst | QDir::IgnoreCase)) {
        listing.append(QString("<li><a href=\"%1%2\">%1%2</a></li>")
                .arg(info.fileName().toHtmlEscaped())
                .arg(info.isDir() ? "/" : ""));
    }

    // Build the response and convert the string to UTF-8
    QByteArray data = ListTemplate
            .arg("/" + path.toHtmlEscaped())
            .arg(listing)
            .arg(QHTTPENGINE_VERSION)
            .toUtf8();

    // Set the headers and write the content
    socket->setHeader("Content-Type", "text/html");
    socket->setHeader("Content-Length", QByteArray::number(data.length()));
    socket->write(data);
    socket->close();
}

FilesystemHandler::FilesystemHandler(QObject *parent)
    : Handler(parent),
      d(new FilesystemHandlerPrivate(this))
{
}

FilesystemHandler::FilesystemHandler(const QString &documentRoot, QObject *parent)
    : Handler(parent),
      d(new FilesystemHandlerPrivate(this))
{
    setDocumentRoot(documentRoot);
}

void FilesystemHandler::setDocumentRoot(const QString &documentRoot)
{
    d->documentRoot.setPath(documentRoot);
}

void FilesystemHandler::process(Socket *socket, const QString &path)
{
    // If a document root is not set, an error has occurred
    if (d->documentRoot.path().isNull()) {
        socket->writeError(Socket::InternalServerError);
        return;
    }

    // URL-decode the path
    QString decodedPath = QUrl::fromPercentEncoding(path.toUtf8());

    // Attempt to retrieve the absolute path
    QString absolutePath;
    if (!d->absolutePath(decodedPath, absolutePath)) {
        socket->writeError(Socket::NotFound);
        return;
    }

    if (QFileInfo(absolutePath).isDir()) {
        d->processDirectory(socket, decodedPath, absolutePath);
    } else {
        d->processFile(socket, absolutePath);
    }
}
