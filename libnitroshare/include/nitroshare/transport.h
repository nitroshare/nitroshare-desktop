/*
 * The MIT License (MIT)
 *
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

#ifndef LIBNITROSHARE_TRANSPORT_H
#define LIBNITROSHARE_TRANSPORT_H

#include <QObject>

#include <nitroshare/config.h>

class Packet;

/**
 * @brief Method for transmitting bundles to other devices
 *
 * Data is sent to another device as packets using a transport. A transport
 * could be as a TCP socket but could also be an encrypted TLS session. Once
 * the connection succeeds and is ready for data transfer, the connected()
 * signal is emitted.
 *
 * In order to avoid blocking the UI thread, all of the virtual methods are
 * invoked asynchronously. Errors are communicated through the error() signal.
 */
class NITROSHARE_EXPORT Transport : public QObject
{
    Q_OBJECT

public:

    /**
     * @brief Send a packet on the transport
     * @param packet pointer to Packet
     */
    virtual void sendPacket(Packet *packet) = 0;

    /**
     * @brief Disconnect and close the transport.
     */
    virtual void close() = 0;

Q_SIGNALS:

    /**
     * @brief Indicate that the transport is ready for use
     */
    void connected();

    /**
     * @brief Indicate that a packet has been received from the transport
     * @param packet pointer to Packet
     */
    void packetReceived(Packet *packet);

    /**
     * @brief Indicate that the previous packet has been sent
     *
     * This signals to the transfer code that the next packet may be sent
     * through the transport.
     */
    void packetSent();

    /**
     * @brief Indicate an error has occurred
     * @param message description of the error
     */
    void error(const QString &message);
};

#endif // LIBNITROSHARE_TRANSPORT_H
