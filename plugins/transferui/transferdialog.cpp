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

#include <QHBoxLayout>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QModelIndexList>
#include <QSpacerItem>
#include <QVBoxLayout>

#include <nitroshare/application.h>
#include <nitroshare/transfer.h>
#include <nitroshare/transfermodel.h>

#include "transferdialog.h"

TransferDialog::TransferDialog(Application *application)
    : mApplication(application),
      mTableView(new QTableView),
      mStopButton(new QPushButton(tr("Stop"))),
      mDismissButton(new QPushButton(tr("Dismiss"))),
      mDismissAllButton(new QPushButton(tr("Dismiss All")))
{
    setWindowTitle(tr("Transfers"));
    resize(800, 300);

    mModel.setSourceModel(mApplication->transferModel());

    mTableView->setModel(&mModel);
    mTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    mTableView->horizontalHeader()->setStretchLastSection(true);
    mTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    mTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    mTableView->verticalHeader()->setVisible(false);

    connect(&mModel, &TransferProxyModel::dataChanged, this, &TransferDialog::updateButtons);
    connect(&mModel, &TransferProxyModel::rowsRemoved, this, &TransferDialog::updateButtons);
    connect(mTableView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &TransferDialog::updateButtons);

    mStopButton->setEnabled(false);
    connect(mStopButton, &QPushButton::clicked, this, &TransferDialog::onStop);

    mDismissButton->setEnabled(false);
    connect(mDismissButton, &QPushButton::clicked, this, &TransferDialog::onDismiss);
    connect(mDismissAllButton, &QPushButton::clicked, this, &TransferDialog::onDismissAll);

    QVBoxLayout *vboxLayout = new QVBoxLayout;
    vboxLayout->addWidget(mStopButton);
    vboxLayout->addWidget(mDismissButton);
    vboxLayout->addWidget(mDismissAllButton);
    vboxLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));

    QHBoxLayout *hboxLayout = new QHBoxLayout;
    hboxLayout->addWidget(mTableView);
    hboxLayout->addLayout(vboxLayout);
    setLayout(hboxLayout);
}

void TransferDialog::updateButtons()
{
    QModelIndex index = currentIndex();

    Transfer *transfer = nullptr;
    if (index.isValid()) {
        transfer = index.data(Qt::UserRole).value<Transfer*>();
    }

    mStopButton->setEnabled(transfer && !transfer->isFinished());
    mDismissButton->setEnabled(transfer && transfer->isFinished());
}

void TransferDialog::onStop()
{
    QModelIndex index = currentIndex();
    if (index.isValid()) {
        index.data(Qt::UserRole).value<Transfer*>()->cancel();
        mTableView->selectionModel()->clear();
    }
}

void TransferDialog::onDismiss()
{
    QModelIndex index = currentIndex();
    if (index.isValid()) {
        mApplication->transferModel()->dismiss(index.row());
    }
}

void TransferDialog::onDismissAll()
{
    mApplication->transferModel()->dismissAll();
}

QModelIndex TransferDialog::currentIndex() const
{
    QModelIndexList selection = mTableView->selectionModel()->selectedIndexes();
    return selection.size() ? selection.at(0) : QModelIndex();
}
