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

#include <QPushButton>
#include <QVBoxLayout>

#include <nitroshare/application.h>
#include <nitroshare/logger.h>
#include <nitroshare/message.h>

#include "logdialog.h"

LogDialog::LogDialog(Application *application)
    : mTextEdit(new QTextEdit)
{
    setWindowTitle(tr("View Log"));
    resize(640, 480);

    // Prepare the text edit
    mTextEdit->setReadOnly(true);
    mTextEdit->setStyleSheet("QTextEdit { font-family: monospace; }");

    // Log existing messages
    foreach (Message *message, application->logger()->messages()) {
        onMessageLogged(message);
    }

    // Log all new messages
    connect(application->logger(), &Logger::messageLogged, this, &LogDialog::onMessageLogged);

    // Create the button for clearing the edit
    QPushButton *pushButton = new QPushButton(tr("Clear"));
    connect(pushButton, &QPushButton::clicked, [this]() {
        mTextEdit->clear();
    });

    // Create the layout and add the widgets
    QVBoxLayout *vboxLayout = new QVBoxLayout;
    vboxLayout->addWidget(mTextEdit);
    vboxLayout->addWidget(pushButton);
    setLayout(vboxLayout);
}

void LogDialog::onMessageLogged(const Message *message)
{
    switch (message->type()) {
    case Message::Error:
        mTextEdit->setTextColor(QColor(Qt::darkRed));
        break;
    case Message::Warning:
        mTextEdit->setTextColor(QColor(Qt::darkYellow));
        break;
    case Message::Info:
        mTextEdit->setTextColor(QColor(Qt::darkBlue));
        break;
    default:
        mTextEdit->setTextColor(QColor(Qt::gray));
        break;
    }

    mTextEdit->append(message->toString());
}
