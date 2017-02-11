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

#ifndef LIBNITROSHARE_SIGNALNOTIFIER_H
#define LIBNITROSHARE_SIGNALNOTIFIER_H

#include <QObject>

#include <nitroshare/config.h>

class NITROSHARE_EXPORT SignalNotifierPrivate;

/**
 * @brief Notification for OS signals
 *
 * Listening for signals from the operating system is platform-dependant and
 * this class takes care of abstracting them as well as converting them to Qt
 * signals.
 *
 * Do not create more than one instance of this class.
 */
class NITROSHARE_EXPORT SignalNotifier : public QObject
{
    Q_OBJECT

public:

    /**
     * @brief Create a new signal notifier
     * @param parent QObject
     */
    explicit SignalNotifier(QObject *parent = nullptr);

Q_SIGNALS:

    /**
     * @brief Indicate that a signal has been caught
     */
    void signal();

private:

    SignalNotifierPrivate *const d;
};

#endif // LIBNITROSHARE_SIGNALNOTIFIER_H
