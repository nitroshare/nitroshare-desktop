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

#include "transferserver.h"

TransferServer::TransferServer(QObject *parent)
    : QTcpServer(parent)
{
    connect(Settings::instance(), &Settings::settingChanged, this, &TransferServer::settingChanged);

    // TODO: listen() can fail without notifying the user

    reload();
}

void TransferServer::settingChanged(Settings::Key key)
{
    if(key == Settings::TransferPort) {
        reload();
    }
}

void TransferServer::incomingConnection(qintptr socketDescriptor)
{
    emit newTransfer(socketDescriptor);
}

void TransferServer::reload()
{
    close();
    listen(QHostAddress::Any, Settings::get<quint16>(Settings::TransferPort));
}
