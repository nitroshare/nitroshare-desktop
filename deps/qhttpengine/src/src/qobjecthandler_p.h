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

#ifndef QHTTPENGINE_QOBJECTHANDLER_P_H
#define QHTTPENGINE_QOBJECTHANDLER_P_H

#include <QMap>
#include <QObject>

namespace QHttpEngine
{

class Socket;
class QObjectHandler;

class QObjectHandlerPrivate : public QObject
{
    Q_OBJECT

public:

    explicit QObjectHandlerPrivate(QObjectHandler *handler);

    // In order to invoke the slot, a "pointer" to it needs to be stored in a
    // map that lets us look up information by method name

    class Method {
    public:
        Method() {}
        Method(QObject *receiver, const char *method, bool readAll)
            : receiver(receiver), oldSlot(true), slot(method), readAll(readAll) {}
        Method(QObject *receiver, QtPrivate::QSlotObjectBase *slotObj, bool readAll)
            : receiver(receiver), oldSlot(false), slot(slotObj), readAll(readAll) {}

        QObject *receiver;
        bool oldSlot;
        union slot{
            slot() {}
            slot(const char *method) : method(method) {}
            slot(QtPrivate::QSlotObjectBase *slotObj) : slotObj(slotObj) {}
            const char *method;
            QtPrivate::QSlotObjectBase *slotObj;
        } slot;
        bool readAll;
    };

    void invokeSlot(Socket*socket, Method m);

    QMap<QString, Method> map;

private:

    QObjectHandler *const q;
};

}

#endif // QHTTPENGINE_QOBJECTHANDLER_P_H
