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

#include <QTcpSocket>
#include <QtEndian>

class SocketStreamPrivate;

/**
 * @brief Synchronous stream for socket I/O
 *
 * The SocketStream class provides a set of methods that enable synchronous
 * operations on sockets. Each of the read and write methods enters an event
 * loop that blocks until the operation is complete or an error occurs.
 *
 * If an error occurs before the expected data is read or written, an exception
 * of type QString is thrown. The string contains a description of the error.
 */
class SocketStream : public QObject
{
    Q_OBJECT

public:

    /**
     * @brief Create a socket stream
     * @param socket socket to use for I/O
     */
    SocketStream(QTcpSocket *socket);

    /**
     * @brief Wait for connection to complete
     * @return true if the connection completed without error
     */
    bool waitForConnected();

    /**
     * @brief Read raw data from the socket
     * @param data storage for data read from the socket
     * @param length length of data to read
     */
    void read(char *data, qint32 length);

    /**
     * @brief Read an integer from the socket
     * @return integer value
     */
    template <typename T>
    T readInt() {
        T value;
        read(reinterpret_cast<char *>(&value), sizeof(value));
        return qFromLittleEndian(value);
    }

    /**
     * @brief Read a QByteArray from the socket
     * @return QByteArray value
     */
    QByteArray readQByteArray();

    /**
     * @brief Write raw data to the socket
     * @param data data to write
     * @param length length of data to write
     */
    void write(const char *data, qint32 length);

    /**
     * @brief Write an integer to the socket
     * @param value integer value
     */
    template <typename T>
    void writeInt(T value) {
        value = qToLittleEndian(value);
        write(reinterpret_cast<const char *>(&value), sizeof(value));
    }

    /**
     * @brief Write a QByteArray to the socket
     * @param value QByteArray value
     */
    void writeQByteArray(const QByteArray &value);

public Q_SLOTS:

    /**
     * @brief Abort any pending operations.
     */
    void abort();

private:

    SocketStreamPrivate *const d;
};

#endif // NS_SOCKETSTREAM_H
