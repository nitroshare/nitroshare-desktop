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

#include <QFont>
#include <QVBoxLayout>
#include <QTextEdit>

#include <nitroshare/application.h>
#include <nitroshare/logger.h>

#include "logdialog.h"

// TODO: this was copied from StderrWriter

QMap<Logger::MessageType, QString> MessageTypeMap = {
    { Logger::Debug, "d" },
    { Logger::Info, "i" },
    { Logger::Warning, "w" },
    { Logger::Error, "e" }
};

LogDialog::LogDialog(Application *application)
{
    setWindowTitle(tr("View Log"));
    resize(640, 480);

    // Create the text edit
    QTextEdit *textEdit = new QTextEdit;
    textEdit->setReadOnly(true);

    // Select a monospace font
    QFont font = textEdit->font();
    font.setFamily("");
    font.setStyleHint(QFont::Monospace);
    textEdit->setFont(font);

    // Log all messages to the dialog
    connect(application->logger(), &Logger::messageLogged,
            [textEdit](Logger::MessageType messageType, const QString &tag, const QString &message) {
        textEdit->append(
            QString("[%1:%2] %3").arg(MessageTypeMap.value(messageType)).arg(tag).arg(message)
        );
    });

    // Create the layout and add the text edit
    QVBoxLayout *vboxLayout = new QVBoxLayout;
    vboxLayout->addWidget(textEdit);
    setLayout(vboxLayout);
}
