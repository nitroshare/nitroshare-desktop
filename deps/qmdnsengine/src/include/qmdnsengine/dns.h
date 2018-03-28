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

#ifndef QMDNSENGINE_DNS_H
#define QMDNSENGINE_DNS_H

#include <QByteArray>
#include <QMap>

#include "qmdnsengine_export.h"

namespace QMdnsEngine
{

class Message;
class Record;

enum {
    /// IPv4 address record
    A = 1,
    /// IPv6 address record
    AAAA = 28,
    /// Wildcard for cache lookups
    ANY = 255,
    /// List of records
    NSEC = 47,
    /// Pointer to hostname
    PTR = 12,
    /// %Service information
    SRV = 33,
    /// Arbitrary metadata
    TXT = 16
};

/**
 * @brief Parse a name from a raw DNS packet
 * @param packet raw DNS packet data
 * @param offset offset into the packet where the name begins
 * @param name reference to QByteArray to store the name in
 * @return true if no errors occurred
 *
 * The offset will be incremented by the number of bytes read. Name
 * compression requires access to the contents of the packet.
 */
QMDNSENGINE_EXPORT bool parseName(const QByteArray &packet, quint16 &offset, QByteArray &name);

/**
 * @brief Write a name to a raw DNS packet
 * @param packet raw DNS packet to write to
 * @param offset offset to update with the number of bytes written
 * @param name name to write to the packet
 * @param nameMap map of names already written to their offsets
 *
 * The offset will be incremented by the number of bytes read. The name map
 * will be updated with offsets of any names written so that it can be passed
 * to future invocations of this function.
 */
QMDNSENGINE_EXPORT void writeName(QByteArray &packet, quint16 &offset, const QByteArray &name, QMap<QByteArray, quint16> &nameMap);

/**
 * @brief Parse a record from a raw DNS packet
 * @param packet raw DNS packet data
 * @param offset offset into the packet where the record begins
 * @param record reference to Record to populate
 * @return true if no errors occurred
 */
QMDNSENGINE_EXPORT bool parseRecord(const QByteArray &packet, quint16 &offset, Record &record);

/**
 * @brief Write a record to a raw DNS packet
 * @param packet raw DNS packet to write to
 * @param offset offset to update with the number of bytes written
 * @param record record to write to the packet
 * @param nameMap map of names already written to their offsets
 */
QMDNSENGINE_EXPORT void writeRecord(QByteArray &packet, quint16 &offset, Record &record, QMap<QByteArray, quint16> &nameMap);

/**
 * @brief Populate a Message with data from a raw DNS packet
 * @param packet raw DNS packet data
 * @param message reference to Message to populate
 * @return true if no errors occurred
 */
QMDNSENGINE_EXPORT bool fromPacket(const QByteArray &packet, Message &message);

/**
 * @brief Create a raw DNS packet from a Message
 * @param message Message to create the packet from
 * @param packet storage for raw DNS packet
 */
QMDNSENGINE_EXPORT void toPacket(const Message &message, QByteArray &packet);

/**
 * @brief Retrieve the string representation of a DNS type
 * @param type integer type
 * @return human-readable name for the type
 */
QMDNSENGINE_EXPORT QString typeName(quint16 type);

}

#endif // QMDNSENGINE_DNS_H
