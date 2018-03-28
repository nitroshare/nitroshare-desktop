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
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLineEdit>
#include <QListView>
#include <QListWidget>
#include <QPushButton>
#include <QSplitter>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QWidget>

#include <qmdnsengine/mdns.h>
#include <qmdnsengine/service.h>

#include "mainwindow.h"
#include "servicemodel.h"

Q_DECLARE_METATYPE(QMdnsEngine::Service)

MainWindow::MainWindow()
    : mServiceModel(nullptr),
      mResolver(nullptr)
{
    setWindowTitle(tr("mDNS Browser"));
    resize(640, 480);

    mServiceType = new QLineEdit(tr("_http._tcp.local."));
    mStartStop = new QPushButton(tr("Browse"));
    mServices = new QListView;
    mAddresses = new QListWidget;
    mAttributes = new QTableWidget;
    mAttributes->setSelectionBehavior(QAbstractItemView::SelectRows);

    QVBoxLayout *rootLayout = new QVBoxLayout;
    QWidget *widget = new QWidget;
    widget->setLayout(rootLayout);
    setCentralWidget(widget);

    QCheckBox *any = new QCheckBox(tr("Any"));

    QHBoxLayout *typeLayout = new QHBoxLayout;
    typeLayout->addWidget(mServiceType, 1);
    typeLayout->addWidget(any);
    typeLayout->addWidget(mStartStop);
    rootLayout->addLayout(typeLayout);

    QSplitter *vSplitter = new QSplitter;
    vSplitter->setOrientation(Qt::Vertical);
    vSplitter->addWidget(mAddresses);
    vSplitter->addWidget(mAttributes);

    QSplitter *hSplitter = new QSplitter;
    hSplitter->addWidget(mServices);
    hSplitter->addWidget(vSplitter);

    QHBoxLayout *servicesLayout = new QHBoxLayout;
    servicesLayout->addWidget(hSplitter);
    rootLayout->addLayout(servicesLayout);

    connect(any, &QCheckBox::toggled, this, &MainWindow::onToggled);
    connect(mStartStop, &QPushButton::clicked, this, &MainWindow::onClicked);
}

void MainWindow::onToggled(bool checked)
{
    if (checked) {
        mServiceType->setText(QMdnsEngine::MdnsBrowseType);
    }
    mServiceType->setEnabled(!checked);
}

void MainWindow::onClicked()
{
    if (mServiceModel) {
        mServices->setModel(nullptr);
        delete mServiceModel;
        mAttributes->clear();
        mAttributes->setColumnCount(0);
    }

    mServiceModel = new ServiceModel(&mServer, mServiceType->text().toUtf8());
    mServices->setModel(mServiceModel);

    connect(mServices->selectionModel(), &QItemSelectionModel::selectionChanged, this, &MainWindow::onSelectionChanged);
}

void MainWindow::onSelectionChanged(const QItemSelection &selected, const QItemSelection &)
{
    mAddresses->clear();
    mAttributes->clear();
    mAttributes->setColumnCount(0);

    if (mResolver) {
        delete mResolver;
        mResolver = nullptr;
    }

    if (selected.count()) {
        auto service = mServiceModel->data(selected.at(0).topLeft(), Qt::UserRole).value<QMdnsEngine::Service>();

        // Show TXT values
        auto attributes = service.attributes();
        mAttributes->setRowCount(attributes.keys().count());
        mAttributes->setColumnCount(2);
        mAttributes->setHorizontalHeaderLabels({tr("Key"), tr("Value")});
        mAttributes->horizontalHeader()->setStretchLastSection(true);
        mAttributes->verticalHeader()->setVisible(false);
        int row = 0;
        for (auto i = attributes.constBegin(); i != attributes.constEnd(); ++i, ++row) {
            mAttributes->setItem(row, 0, new QTableWidgetItem(QString(i.key())));
            mAttributes->setItem(row, 1, new QTableWidgetItem(QString(i.value())));
        }

        // Resolve the address
        mResolver = new QMdnsEngine::Resolver(&mServer, service.hostname(), nullptr, this);
        connect(mResolver, &QMdnsEngine::Resolver::resolved, [this](const QHostAddress &address) {
            mAddresses->addItem(address.toString());
        });
    }
}
