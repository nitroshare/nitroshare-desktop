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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <qmdnsengine/server.h>
#include <qmdnsengine/hostname.h>
#include <qmdnsengine/message.h>
#include <qmdnsengine/provider.h>

class QCheckBox;
class QPushButton;
class QLineEdit;
class QTextEdit;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    MainWindow();

private Q_SLOTS:

    void onClicked();
    void onHostnameChanged(const QByteArray &hostname);
    void onMessageReceived(const QMdnsEngine::Message &message);

private:

    QString buttonCaption() const;

    QMdnsEngine::Server mServer;
    QMdnsEngine::Hostname mHostname;
    QMdnsEngine::Provider *mProvider;

    QLineEdit *mServiceName;
    QLineEdit *mServiceType;
    QLineEdit *mServicePort;

    QPushButton *mButton;
    QCheckBox *mShowQueries;

    QTextEdit *mLog;
};

#endif // MAINWINDOW_H
