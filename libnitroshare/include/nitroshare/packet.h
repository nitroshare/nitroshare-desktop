/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Nathan Osman
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

#ifndef LIBNITROSHARE_PACKET_H
#define LIBNITROSHARE_PACKET_H

#include <QObject>

#include <nitroshare/config.h>

class NITROSHARE_EXPORT PacketPrivate;

/**
 * @brief A fragment of metadata or item content for transfer
 *
 * Each packet includes its type and payload. Packets that convey information
 * about state often have empty payloads.
 */
class NITROSHARE_EXPORT Packet : public QObject
{
    Q_OBJECT
    Q_ENUMS(Type)
    Q_PROPERTY(Type type READ type)
    Q_PROPERTY(QByteArray content READ content)

public:

    /**
     * @brief Packet type
     */
    enum Type {
        /// Sent by receiving peer to indicate successful transfer
        Success = 0,
        /// Sent by either peer to indicate an error and abort the transfer
        Error,
        /// JSON metadata
        Json,
        /// Binary data (item content)
        Binary
    };

    /**
     * @brief Create a new packet
     * @param type one of Type
     * @param content payload
     * @param parent QObject
     */
    Packet(Type type, const QByteArray &content = QByteArray(), QObject *parent = nullptr);

    /**
     * @brief Retrieve the packet type
     * @return one of Type
     */
    Type type() const;

    /**
     * @brief Retrieve the content of the packet
     * @return payload
     */
    QByteArray content() const;

private:

    PacketPrivate *const d;
};

#endif // LIBNITROSHARE_PACKET_H
