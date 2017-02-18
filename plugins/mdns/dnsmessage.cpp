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

#include "dnsmessage.h"

DnsMessage::DnsMessage(const QByteArray &message)
{
    mOkay = parse(message);
}

bool DnsMessage::isOkay() const
{
    return mOkay;
}

QList<DnsMessage::Question> DnsMessage::questions() const
{
    return mQuestions;
}

QList<DnsMessage::Answer> DnsMessage::answers() const
{
    return mAnswers;
}

bool DnsMessage::parse(const QByteArray &message)
{
    quint16 offset = 0;
    quint16 transactionId, flags, nQuestions, nAnswers, nAuthRecs, nAddRecs;
    if (!parseInteger<quint16>(message, offset, transactionId) ||
            !parseInteger<quint16>(message, offset, flags) ||
            !parseInteger<quint16>(message, offset, nQuestions) ||
            !parseInteger<quint16>(message, offset, nAnswers) ||
            !parseInteger<quint16>(message, offset, nAuthRecs) ||
            !parseInteger<quint16>(message, offset, nAddRecs)) {
        return false;
    }
    for (int i = 0; i < nQuestions; ++i) {
        if (!parseQuestion(message, offset)) {
            return false;
        }
    }
    for (int i = 0; i < nAnswers; ++i) {
        if (!parseAnswer(message, offset)) {
            return false;
        }
    }
    return true;
}

bool DnsMessage::parseQuestion(const QByteArray &message, quint16 &offset)
{
    QByteArray name;
    if (!parseName(message, offset, name)) {
        return false;
    }
    quint16 type, class_;
    if (!parseInteger<quint16>(message, offset, type) ||
            !parseInteger<quint16>(message, offset, class_)) {
        return false;
    }
    mQuestions.append(Question{name, type});
    return true;
}

bool DnsMessage::parseAnswer(const QByteArray &message, quint16 &offset)
{
    QByteArray name;
    if (!parseName(message, offset, name)) {
        return false;
    }
    quint16 type, class_, nBytes;
    quint32 ttl;
    if (!parseInteger<quint16>(message, offset, type) ||
            !parseInteger<quint16>(message, offset, class_) ||
            !parseInteger<quint32>(message, offset, ttl) ||
            !parseInteger<quint16>(message, offset, nBytes) ||
            offset + nBytes > message.length()) {
        return false;
    }
    offset += nBytes;
    mAnswers.append(Answer{name, type, ttl});
    return true;
}

bool DnsMessage::parseName(const QByteArray &message, quint16 &offset, QByteArray &name)
{
    quint16 offsetEnd = 0;
    quint16 offsetPtr = offset;
    forever {
        if (offset >= message.length()) {
            return false;  // out-of-bounds
        }
        quint8 nBytes;
        if (!parseInteger<quint8>(message, offset, nBytes)) {
            return false;
        }
        if (!nBytes) {
            break;
        }
        quint8 nBytes2;
        quint16 newOffset;
        switch (nBytes & 0xc0) {
        case 0x00:
            if (offset + nBytes > message.length()) {
                return false;  // length exceeds message
            }
            name.append(message.mid(offset, nBytes));
            name.append('.');
            offset += nBytes;
            break;
        case 0xc0:
            if (!parseInteger<quint8>(message, offset, nBytes2)) {
                return false;
            }
            newOffset = ((nBytes & ~0xc0) << 8) | nBytes2;
            if (newOffset >= offsetPtr) {
                return false;  // prevent infinite loop
            }
            offsetPtr = newOffset;
            if (!offsetEnd) {
                offsetEnd = offset;
            }
            offset = newOffset;
            break;
        default:
            return false;  // no other types supported
        }
    }
    if (offsetEnd) {
        offset = offsetEnd;
    }
    return true;
}
