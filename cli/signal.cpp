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

#include <sys/signal.h>
#include <sys/socket.h>
#include <unistd.h>

#include <QSocketNotifier>

#include "signal.h"

// Global instance
Signal GlobalSignal;

// Socket pair used for triggering signal
int pair[2];

// Signal handler for writing data to the pipe
void onSignal(int)
{
    char c = 0;
    write(pair[0], &c, sizeof(c));
}

Signal::Signal()
{
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, pair) == 0) {
        struct sigaction action = {};
        action.sa_handler = onSignal;
        sigemptyset(&action.sa_mask);
        action.sa_flags = SA_RESTART;
        if (sigaction(SIGINT, &action, 0) == 0 && sigaction(SIGTERM, &action, 0) == 0) {
            connect(
                new QSocketNotifier(pair[1], QSocketNotifier::Read, this),
                &QSocketNotifier::activated,
                this,
                &Signal::signal
            );
        }
    }
}

Signal *Signal::instance()
{
    return &GlobalSignal;
}
