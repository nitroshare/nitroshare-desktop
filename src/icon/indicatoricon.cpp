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

#include <QAction>

#undef signals
#include <libnotify/notify.h>

#include "indicatoricon.h"

// Assumes 'data' is a QAction * and triggers it
void triggerQAction(GtkMenu *, gpointer data)
{
    reinterpret_cast<QAction *>(data)->trigger();
}

IndicatorIcon::IndicatorIcon()
    : mIndicator(app_indicator_new("nitroshare", "nitroshare", APP_INDICATOR_CATEGORY_APPLICATION_STATUS)),
      mMenu(gtk_menu_new())
{
    notify_init("nitroshare");

    app_indicator_set_menu(mIndicator, GTK_MENU(mMenu));
    app_indicator_set_status(mIndicator, APP_INDICATOR_STATUS_ACTIVE);
}

IndicatorIcon::~IndicatorIcon()
{
    notify_uninit();

    g_object_unref(G_OBJECT(mIndicator));
}

void IndicatorIcon::addAction(const QString &text, QObject *receiver, const char *member)
{
    GtkWidget *menuItem(gtk_menu_item_new_with_label(text.toUtf8().constData()));

    QAction *action(new QAction(text, this));
    connect(action, SIGNAL(triggered()), receiver, member);

    g_signal_connect(menuItem, "activate", G_CALLBACK(triggerQAction), action);

    gtk_widget_show(menuItem);
    gtk_menu_shell_append(GTK_MENU_SHELL(mMenu), menuItem);
}

void IndicatorIcon::addSeparator()
{
    GtkWidget *separator(gtk_separator_menu_item_new());

    gtk_widget_show(separator);
    gtk_menu_shell_append(GTK_MENU_SHELL(mMenu), separator);
}

void IndicatorIcon::showMessage(const QString &message)
{
    NotifyNotification *notification(notify_notification_new(
        "NitroShare Notification",
        message.toUtf8().constData(),
        "nitroshare"
    ));

    notify_notification_show(notification, nullptr);

    g_object_unref(G_OBJECT(notification));
}
