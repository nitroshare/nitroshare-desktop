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

#include <QCheckBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>

#include <qmdnsengine/dns.h>
#include <qmdnsengine/query.h>
#include <qmdnsengine/service.h>

#include "mainwindow.h"

MainWindow::MainWindow()
    : mHostname(&mServer),
      mProvider(0),
      mServiceName(new QLineEdit(tr("Test Service"))),
      mServiceType(new QLineEdit(tr("_test._tcp.local."))),
      mServicePort(new QLineEdit("1234")),
      mButton(new QPushButton(buttonCaption())),
      mShowQueries(new QCheckBox(tr("Show queries"))),
      mLog(new QTextEdit(tr("Initializing application")))
{
    setWindowTitle(tr("mDNS Provider"));
    resize(640, 480);

    mServicePort->setValidator(new QIntValidator(1, 65535, this));
    mLog->setReadOnly(true);

    // Create the root layout
    QVBoxLayout *rootLayout = new QVBoxLayout;
    QWidget *widget = new QWidget;
    widget->setLayout(rootLayout);
    setCentralWidget(widget);

    // Create the horizontal layout for the grid and buttons
    QHBoxLayout *upperLayout = new QHBoxLayout;
    rootLayout->addLayout(upperLayout);

    // Create the grid layout for the line edits
    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->addWidget(new QLabel(tr("Service name:")), 0, 0);
    gridLayout->addWidget(mServiceName, 0, 1);
    gridLayout->addWidget(new QLabel(tr("Service type:")), 1, 0);
    gridLayout->addWidget(mServiceType, 1, 1);
    gridLayout->addWidget(new QLabel(tr("Service port:")), 2, 0);
    gridLayout->addWidget(mServicePort, 2, 1);
    upperLayout->addLayout(gridLayout);

    // Create the layout for the buttons
    QVBoxLayout *buttonLayout = new QVBoxLayout;
    buttonLayout->addWidget(mButton);
    buttonLayout->addWidget(mShowQueries);
    buttonLayout->addWidget(new QWidget, 1);
    upperLayout->addLayout(buttonLayout);

    // Add the log
    rootLayout->addWidget(mLog, 1);

    connect(mButton, &QPushButton::clicked, this, &MainWindow::onClicked);
    connect(&mHostname, &QMdnsEngine::Hostname::hostnameChanged, this, &MainWindow::onHostnameChanged);
    connect(&mServer, &QMdnsEngine::Server::messageReceived, this, &MainWindow::onMessageReceived);
}

void MainWindow::onClicked()
{
    if (mProvider) {
        mLog->append(tr("Destroying provider"));
        delete mProvider;
        mProvider = 0;
    } else {
        mLog->append(tr("Creating provider"));
        QMdnsEngine::Service service;
        service.setName(mServiceName->text().toUtf8());
        service.setType(mServiceType->text().toUtf8());
        service.setPort(mServicePort->text().toUShort());
        mProvider = new QMdnsEngine::Provider(&mServer, &mHostname, this);
        mProvider->update(service);
    }
    mButton->setText(buttonCaption());
}

void MainWindow::onHostnameChanged(const QByteArray &hostname)
{
    mLog->append(tr("Hostname changed to %1").arg(QString(hostname)));
}

void MainWindow::onMessageReceived(const QMdnsEngine::Message &message)
{
    if (!mShowQueries->isChecked()) {
        return;
    }
    foreach (QMdnsEngine::Query query, message.queries()) {
        mLog->append(
            tr("[%1] %2")
                .arg(QMdnsEngine::typeName(query.type()))
                .arg(QString(query.name()))
        );
    }
}

QString MainWindow::buttonCaption() const
{
    return mProvider ? tr("Stop") : tr("Start");
}
