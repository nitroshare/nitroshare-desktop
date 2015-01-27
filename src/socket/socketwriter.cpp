/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Nathan Osman
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
 **/

#include <QFile>
#include <QTcpSocket>

#include "../util/settings.h"
#include "socketstream.h"
#include "socketwriter.h"

SocketWriter::SocketWriter(const QHostAddress &address, quint16 port, BundlePointer bundle)
    : mAddress(address), mPort(port), mBundle(bundle)
{
}

void SocketWriter::start()
{
    QTcpSocket socket;
    SocketStream stream(socket);

    try {
        socket.connectToHost(mAddress, mPort);

        if(!stream.waitForConnected()) {
            throw tr("Unable to connect to host.");
        }

        stream.writeInt<qint32>(1);
        stream.writeQByteArray(Settings::get<QString>(Settings::DeviceName).toUtf8());
        stream.writeInt<qint64>(mBundle->totalSize());
        stream.writeInt<qint32>(mBundle->count());

        qint64 bufferSize(Settings::get<qint64>(Settings::TransferBuffer));
        char buffer[bufferSize];

        foreach(FileInfo info, *mBundle) {
            stream.writeQByteArray(info.filename().toUtf8());

            QFile file(info.absoluteFilename());
            if(!file.open(QIODevice::ReadOnly)) {
                throw tr("Unable to open %1 for reading.").arg(info.absoluteFilename());
            }

            qint64 fileSize(file.size());
            stream.writeInt<qint64>(fileSize);

            while(fileSize) {
                qint64 lengthRead(file.read(buffer, qMin(fileSize, bufferSize)));

                if(lengthRead <= 0) {
                    throw tr("Unable to read from %1.").arg(info.absoluteFilename());
                }

                stream.writeQByteArray(qCompress(buffer, lengthRead));
                fileSize -= lengthRead;
            }
        }

        emit completed();
    } catch(const QString &exception) {
        emit error(exception);
    }
}
