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

#include <libnotify/notify.h>

#include "showlibnotifynotificationaction.h"

ShowLibnotifyNotificationAction::ShowLibnotifyNotificationAction()
{
    notify_init("nitroshare");
}

ShowLibnotifyNotificationAction::~ShowLibnotifyNotificationAction()
{
    notify_uninit();
}

QString ShowLibnotifyNotificationAction::name() const
{
    return "showlibnotifynotification";
}

QVariant ShowLibnotifyNotificationAction::invoke(const QVariantMap &params)
{
    // Create the notification
    NotifyNotification *notification = notify_notification_new(
        params.value("title").toString().toUtf8().constData(),
        params.value("message").toString().toUtf8().constData(),
        "nitroshare-indicator"
    );

    // Attempt to display it and free the notification
    gboolean ret = notify_notification_show(notification, nullptr);
    g_object_unref(G_OBJECT(notification));

    // Indicate if the call succeeded
    return ret == TRUE;
}
