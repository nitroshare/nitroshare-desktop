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

#ifndef DNSMESSAGE_H
#define DNSMESSAGE_H

#include <QByteArray>
#include <QList>
#include <QtEndian>

/**
 * @brief Incredibly simple DNS message parser and generator
 */
class DnsMessage
{
public:

    struct Question
    {
        QByteArray name;
        quint16 type;
    };

    struct Answer
    {
        QByteArray name;
        quint16 type;
        quint32 ttl;
    };

    explicit DnsMessage(const QByteArray &message);

    bool isOkay() const;

    QList<Question> questions() const;
    QList<Answer> answers() const;

private:

    bool parse(const QByteArray &message);
    bool parseQuestion(const QByteArray &message, quint16 &offset);
    bool parseAnswer(const QByteArray &message, quint16 &offset);
    bool parseName(const QByteArray &message, quint16 &offset, QByteArray &name);

    template<class T>
    bool parseInteger(const QByteArray &message, quint16 &offset, T &value)
    {
        if (offset + sizeof(T) > message.length()) {
            return false;  // out-of-bounds
        }
        value = qFromBigEndian<T>(reinterpret_cast<const uchar*>(message.constData() + offset));
        offset += sizeof(T);
        return true;
    }

    bool mOkay;

    QList<Question> mQuestions;
    QList<Answer> mAnswers;
};

#endif // DNSMESSAGE_H
