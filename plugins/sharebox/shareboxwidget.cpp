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

#include <QApplication>
#include <QBrush>
#include <QColor>
#include <QDesktopWidget>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDropEvent>
#include <QLinearGradient>
#include <QMimeData>
#include <QMouseEvent>
#include <QPainter>
#include <QPen>
#include <QRect>
#include <QUrl>

#include <nitroshare/action.h>
#include <nitroshare/actionregistry.h>
#include <nitroshare/application.h>
#include <nitroshare/logger.h>
#include <nitroshare/message.h>

#include "shareboxwidget.h"

const QString MessageTag = "sharebox";

const int WidgetSize = 100;
const int WidgetMargin = 20;

const QColor NormalColor = QColor::fromRgbF(1.0f, 1.0f, 1.0f, 0.4f);
const QColor ActiveColor = QColor::fromRgbF(1.0f, 1.0f, 1.0f, 1.0f);

const qreal WidgetOpacity = 0.5f;

ShareboxWidget::ShareboxWidget(Application *application)
    : mApplication(application),
      mLogo(":/sharebox/logo.png"),
      mDragActive(false),
      mMoveActive(false)
{
    // Adjust the widget size
    resize(WidgetSize, WidgetSize);

    // Position it
    const QRect screenRect = QApplication::desktop()->availableGeometry();
    move(screenRect.right() - WidgetSize - WidgetMargin,
         screenRect.bottom() - WidgetSize - WidgetMargin);

    // Special attributes for various platforms
    setAttribute(Qt::WA_MacAlwaysShowToolWindow);
    setAttribute(Qt::WA_TranslucentBackground);

    // The flags used below have been found through careful experimentation
    setWindowFlags(
#ifdef Q_OS_WIN32
        Qt::Tool |
#else
        Qt::ToolTip |
#endif
        Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint
    );

    // Make the widget transparent
    setWindowOpacity(WidgetOpacity);

    // Allow things to be dropped
    setAcceptDrops(true);

    // Activate the widget
    show();
}

void ShareboxWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        mDragActive = true;
        setWindowOpacity(1.0f);
        update();

        event->accept();
    }
}

void ShareboxWidget::dragLeaveEvent(QDragLeaveEvent *event)
{
    mDragActive = false;
    setWindowOpacity(WidgetOpacity);
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

    // Build a list of paths to the items
    if (event->mimeData()->hasUrls()) {
        QStringList items;
        foreach (const QUrl &url, event->mimeData()->urls()) {
            items.append(url.toLocalFile());
        }
        sendItems(items);
    }

    // dragLeaveEvent() is not invoked if the item is dropped
    mDragActive = false;
    setWindowOpacity(WidgetOpacity);
    update();
}

void ShareboxWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        mMoveActive = true;
        mOriginalPos = event->pos();
    }
}

void ShareboxWidget::mouseMoveEvent(QMouseEvent *event)
{
    QPoint dest = event->globalPos() - mOriginalPos;

    // Snap the box to a 10-pixel grid
    dest /= 10;
    dest *= 10;

    move(dest);
}

void ShareboxWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        mMoveActive = false;
    }
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

    // Determine the correct color
    QColor color = mDragActive ? ActiveColor : NormalColor;

    // Draw the outline
    painter.setPen(QPen(color, 4.0f, Qt::DotLine, Qt::SquareCap));
    painter.drawRect(rect().adjusted(8, 8, -8, -8));

    // Draw the logo onto a separate pixmap with the correct color
    QPixmap logo = mLogo;
    QPainter pixmapPainter(&logo);
    pixmapPainter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    pixmapPainter.fillRect(logo.rect(), color);
    pixmapPainter.end();

    // Draw the NitroShare logo in the center
    painter.drawPixmap(
        (WidgetSize / 2) - 32,
        (WidgetSize / 2) - 32,
        64, 64,
        logo
    );
}

void ShareboxWidget::sendItems(QStringList items)
{
    // Invoke the device selection dialog to select a device
    QVariant deviceRet = mApplication->actionRegistry()->find("selectdeviceui")->invoke();
    if (deviceRet.type() != QVariant::Map) {
        return;
    }

    // Invoke the send files action to initiate the transfer
    mApplication->actionRegistry()->find("sendfiles")->invoke({
        { "device", deviceRet.toMap().value("device") },
        { "enumerator", deviceRet.toMap().value("enumerator") },
        { "items", items }
    });
}
