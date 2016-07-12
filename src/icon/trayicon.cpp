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

#include <QApplication>
#include <QDesktopWidget>
#include <QIcon>
#include <QPainter>
#include <QPixmap>
#include <QPoint>
#include <QRect>
#include <QScreen>
#include <QSvgRenderer>

#include "trayicon.h"

TrayIcon::TrayIcon()
{
    mTrayIcon.setContextMenu(&mMenu);
    mTrayIcon.setToolTip(tr("NitroShare"));
    mTrayIcon.show();

    // The following code has a single purpose - to determine the appropriate
    // size for the tray icon - this is done by determining the scaling factor
    // for the screen containing the tray icon and setting the size
    // accordingly - simply calling QIcon("[...].svg") doesn't work in KDE

    // Retrieve the geometry for the tray icon
    QRect rect = mTrayIcon.geometry();

    // Obtain the screen for the tray icon
    QScreen *screen = QApplication::screens().at(
        QApplication::desktop()->screenNumber(QPoint(rect.x(), rect.y()))
    );

    // Determine the dimension with the smallest size and scale it
    int minSize = qMin(rect.width(), rect.height());
    minSize *= screen->devicePixelRatio();

    // Create an empty pixmap with that size and make it transparent
    QPixmap pixmap(minSize, minSize);
    pixmap.fill(Qt::transparent);

    // Render the SVG (finally!)
    QSvgRenderer renderer(QString(":/img/tray.svg"));
    QPainter painter(&pixmap);
    renderer.render(&painter);

    // *Now* we can set the icon
    mTrayIcon.setIcon(QIcon(pixmap));
}

void TrayIcon::addAction(const QString &text, QObject *receiver, const char *member)
{
    mMenu.addAction(text, receiver, member);
}

void TrayIcon::addSeparator()
{
    mMenu.addSeparator();
}

void TrayIcon::showMessage(const QString &message)
{
    mTrayIcon.showMessage(tr("NitroShare Notification"), message);
}
