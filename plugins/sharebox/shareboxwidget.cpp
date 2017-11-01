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

#include <QBrush>
#include <QColor>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDropEvent>
#include <QFont>
#include <QLinearGradient>
#include <QMimeData>
#include <QPainter>
#include <QPen>
#include <QUrl>

#include <nitroshare/action.h>
#include <nitroshare/actionregistry.h>
#include <nitroshare/application.h>
#include <nitroshare/logger.h>
#include <nitroshare/message.h>

#include "shareboxwidget.h"

const QString MessageTag = "sharebox";

const QColor NormalColor = QColor::fromRgbF(0.5f, 0.5f, 0.5f);
const QColor ActiveColor = QColor::fromRgbF(1.0f, 1.0f, 1.0f);

ShareboxWidget::ShareboxWidget(Application *application)
    : mApplication(application),
      mDragActive(false)
{
    // Adjust the widget size
    resize(140, 140);

    // Remove the frame from the window and ensure it stays on the desktop
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnBottomHint);
    setAttribute(Qt::WA_X11NetWmWindowTypeDock);

    // Allow things to be dropped
    setAcceptDrops(true);

    // Activate the widget
    show();
}

void ShareboxWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        mDragActive = true;
        update();

        event->accept();
    }
}

void ShareboxWidget::dragLeaveEvent(QDragLeaveEvent *event)
{
    mDragActive = false;
    update();

    event->accept();
}

void ShareboxWidget::dropEvent(QDropEvent *event)
{
    mApplication->logger()->log(new Message(
        Message::Info,
        MessageTag,
        "objects dropped on sharebox"
    ));

    if (event->mimeData()->hasUrls()) {

        // Build a list of items
        QStringList items;
        foreach (const QUrl &url, event->mimeData()->urls()) {
            items.append(url.toLocalFile());
        }

        // Find & invoke the browse action
        Action *browseAction = mApplication->actionRegistry()->find("browse");
        QVariant returnValue = browseAction->invoke();
        if (returnValue.type() != QVariant::Map) {
            return;
        }

        // Find & invoke the send action
        Action *sendAction = mApplication->actionRegistry()->find("send");
        sendAction->invoke({
            { "device", returnValue.toMap().value("device") },
            { "enumerator", returnValue.toMap().value("enumerator") },
            { "items", items }
        });
    }

    // dragLeaveEvent() is not invoked if the item is dropped
    update();
}

void ShareboxWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Draw the top half of the gradient
    QLinearGradient topGradient(0, 0, 0, height() / 2);
    topGradient.setColorAt(0, QColor::fromRgbF(0.4f, 0.4f, 0.4f));
    topGradient.setColorAt(1, QColor::fromRgbF(0.2f, 0.2f, 0.2f));
    painter.fillRect(0, 0, width(), height() / 2, QBrush(topGradient));

    // Draw the bottom half of the gradient
    QLinearGradient bottomGradient(0, height() / 2, 0, height());
    bottomGradient.setColorAt(0, QColor::fromRgbF(0.15f, 0.15f, 0.15f));
    bottomGradient.setColorAt(1, QColor::fromRgbF(0.0f, 0.0f, 0.0f));
    painter.fillRect(0, height() / 2, width(), height(), QBrush(bottomGradient));

    // Draw the outline
    painter.setPen(QPen(mDragActive ? ActiveColor : NormalColor, 4.0f, Qt::DotLine, Qt::SquareCap));
    painter.drawRect(rect().adjusted(10, 10, -10, -10));

    // Draw the text in the center
    QFont font;
    font.setPointSize(24);
    painter.setFont(font);
    painter.drawText(rect(), Qt::AlignHCenter | Qt::AlignVCenter, tr("send"));
}
