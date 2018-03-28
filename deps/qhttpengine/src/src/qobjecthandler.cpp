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

#include <QGenericArgument>
#include <QMetaMethod>

#include <qhttpengine/qobjecthandler.h>
#include <qhttpengine/socket.h>

#include "qobjecthandler_p.h"

using namespace QHttpEngine;

QObjectHandlerPrivate::QObjectHandlerPrivate(QObjectHandler *handler)
    : QObject(handler),
      q(handler)
{
}

QObjectHandler::QObjectHandler(QObject *parent)
    : Handler(parent),
      d(new QObjectHandlerPrivate(this))
{
}

void QObjectHandlerPrivate::invokeSlot(Socket *socket, Method m)
{
    // Invoke the slot
    if (m.oldSlot) {

        // Obtain the slot index
        int index = m.receiver->metaObject()->indexOfSlot(m.slot.method + 1);
        if (index == -1) {
            socket->writeError(Socket::InternalServerError);
            return;
        }

        QMetaMethod method = m.receiver->metaObject()->method(index);

        // Ensure the parameter is correct
        QList<QByteArray> params = method.parameterTypes();
        if (params.count() != 1 || params.at(0) != "QHttpEngine::Socket*") {
            socket->writeError(Socket::InternalServerError);
            return;
        }

        // Invoke the method
        if (!m.receiver->metaObject()->method(index).invoke(
                    m.receiver, Q_ARG(Socket*, socket))) {
            socket->writeError(Socket::InternalServerError);
            return;
        }
    } else {
        void *args[] = {
            Q_NULLPTR,
            &socket
        };
        m.slot.slotObj->call(m.receiver, args);
    }
}

void QObjectHandler::process(Socket *socket, const QString &path)
{
    // Ensure the method has been registered
    if (!d->map.contains(path)) {
        socket->writeError(Socket::NotFound);
        return;
    }

    QObjectHandlerPrivate::Method m = d->map.value(path);

    // If the slot requires all data to be received, check to see if this is
    // already the case, otherwise, wait until the rest of it arrives
    if (!m.readAll || socket->bytesAvailable() >= socket->contentLength()) {
        d->invokeSlot(socket, m);
    } else {
        connect(socket, &Socket::readChannelFinished, [this, socket, m]() {
            d->invokeSlot(socket, m);
        });
    }
}

void QObjectHandler::registerMethod(const QString &name, QObject *receiver, const char *method, bool readAll)
{
    d->map.insert(name, QObjectHandlerPrivate::Method(receiver, method, readAll));
}

void QObjectHandler::registerMethodImpl(const QString &name, QObject *receiver, QtPrivate::QSlotObjectBase *slotObj, bool readAll)
{
    d->map.insert(name, QObjectHandlerPrivate::Method(receiver, slotObj, readAll));
}
