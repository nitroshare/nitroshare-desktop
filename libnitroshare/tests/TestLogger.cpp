/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Nathan Osman
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

#include <QSignalSpy>
#include <QTest>

#include <nitroshare/logger.h>
#include <nitroshare/message.h>

Q_DECLARE_METATYPE(const Message*)

const Message::Type MessageType = Message::Error;
const QString MessageTag = "tag";
const QString MessageBody = "body";

class TestLogger : public QObject
{
    Q_OBJECT

private slots:

    void initTestCase();

    void testLogger();
};

void TestLogger::initTestCase()
{
    qRegisterMetaType<const Message*>();
}

void TestLogger::testLogger()
{
    Logger logger;
    Message *message = new Message(MessageType, MessageTag, MessageBody);

    QSignalSpy messageLoggedSpy(&logger, &Logger::messageLogged);

    // Log the message and verify that the correct signal was emitted
    logger.log(message);
    QCOMPARE(messageLoggedSpy.count(), 1);
    QCOMPARE(messageLoggedSpy.at(0).at(0).value<const Message*>(), message);

    // Check to see if the logger retained the message
    QCOMPARE(logger.messages().count(), 1);
    QCOMPARE(logger.messages().at(0), message);
}

QTEST_MAIN(TestLogger)
#include "TestLogger.moc"
