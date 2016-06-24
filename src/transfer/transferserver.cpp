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
#include "transferserver_p.h"

TransferServerPrivate::TransferServerPrivate(TransferServer *transferServer)
    : QTcpServer(transferServer),
      q(transferServer)
{
    connect(Settings::instance(), &Settings::settingsChanged, this, &TransferServerPrivate::onSettingsChanged);
}

void TransferServerPrivate::onSettingsChanged(const QList<Settings::Key> &keys)
{
    Settings *settings = Settings::instance();

    if (keys.empty() || keys.contains(Settings::Key::BehaviorReceive) ||
            keys.contains(Settings::Key::TransferPort)) {
        close();
        if (settings->get(Settings::Key::BehaviorReceive).toBool()) {
            quint16 port = Settings::instance()->get(Settings::Key::TransferPort).toInt();
            if (!listen(QHostAddress::Any, port)) {
                emit q->error(tr("Unable to listen on port %1").arg(port));
            }
        }
    }
}

void TransferServerPrivate::incomingConnection(qintptr socketDescriptor)
{
    emit q->newTransfer(socketDescriptor);
}

TransferServer::TransferServer(QObject *parent)
    : QObject(parent),
      d(new TransferServerPrivate(this))
{
}

TransferServer::~TransferServer()
{
    delete d;
}

void TransferServer::start()
{
    d->onSettingsChanged();
}
