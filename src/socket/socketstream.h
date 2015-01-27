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

#ifndef NS_SOCKETSTREAM_H
#define NS_SOCKETSTREAM_H

#include <QEventLoop>
#include <QTcpSocket>
#include <QTimer>
#include <QtEndian>

class SocketStream : public QObject
{
    Q_OBJECT

public:

    SocketStream(QTcpSocket &socket);

    template <typename T>
    inline T readInt() {
        T value;
        read(reinterpret_cast<char *>(&value), sizeof(value));
        return qFromLittleEndian(value);
    }

    inline QByteArray readQByteArray() {
        qint32 length(readInt<qint32>());
        QByteArray value;
        value.resize(length);
        read(value.data(), length);
        return value;
    }

    template <typename T>
    inline void writeInt(T value) {
        value = qToLittleEndian(value);
        write(reinterpret_cast<const char *>(&value), sizeof(value));
    }

    inline void writeQByteArray(const QByteArray &value) {
        writeInt<qint32>(value.length());
        write(value.constData(), value.length());
    }

public slots:

    void cancel();

private:

    enum Signal {
        ReadyRead,
        BytesWritten
    };

    void read(char *data, qint32 length);
    void write(const char *data, qint32 length);

    bool waitFor(Signal signal);

    QTcpSocket &mSocket;

    QEventLoop mLoop;
    QTimer mTimer;

    Signal mWaitingFor;
    bool mSucceeded;
};

#endif // NS_SOCKETSTREAM_H
