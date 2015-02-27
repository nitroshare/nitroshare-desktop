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

#include <QPersistentModelIndex>
#include <QProgressBar>
#include <QPushButton>

#include "transferwindow.h"
#include "ui_transferwindow.h"

TransferWindow::TransferWindow(TransferModel *model)
    : ui(new Ui::TransferWindow),
      mModel(model)
{
    ui->setupUi(this);

    ui->transferView->setModel(mModel);

    connect(ui->clear, &QPushButton::clicked, mModel, &TransferModel::clear);
    connect(mModel, &TransferModel::rowsInserted, this, &TransferWindow::onRowsInserted);
    connect(mModel, &TransferModel::dataChanged, this, &TransferWindow::onDataChanged);
}

TransferWindow::~TransferWindow()
{
    delete ui;
}

void TransferWindow::onRowsInserted(const QModelIndex &, int first, int last)
{
    for(int row = first; row <= last; ++row) {

        // Create a progress box for displaying progress
        QProgressBar *progressBar = new QProgressBar;
        progressBar->setMinimum(0);
        progressBar->setMaximum(100);
        ui->transferView->setIndexWidget(mModel->index(row, TransferModel::ProgressColumn), progressBar);

        updateButton(row);
    }
}

void TransferWindow::onDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight,  const QVector<int> &roles)
{
    for(int row = topLeft.row(); row <= bottomRight.row(); ++row) {

        // Update the progress box if its value has changed
        if(roles.contains(TransferModel::ProgressRole) || roles.empty()) {
            updateProgressBar(row);
        }

        // Update the button if the state has changed
        if(roles.contains(TransferModel::StateRole) || roles.isEmpty()) {
            updateButton(row);
        }
    }
}

void TransferWindow::updateProgressBar(int row)
{
    // Retrieve the progress bar
    QModelIndex index = mModel->index(row, TransferModel::ProgressColumn);
    QProgressBar *progressBar = qobject_cast<QProgressBar*>(ui->transferView->indexWidget(index));

    // Set its progress
    int progress = index.data(TransferModel::ProgressRole).toInt();
    progressBar->setValue(progress);
}

void TransferWindow::updateButton(int row)
{
    // Create the button and persistent model index used to reference the row
    QPushButton *button = new QPushButton;
    QPersistentModelIndex index(mModel->index(row, 0));

    // The title and action for the button depend on the direction and state
    int direction = index.data(TransferModel::DirectionRole).toInt();
    int state = index.data(TransferModel::StateRole).toInt();

    // Display:
    // - a cancel button for transfers in progress
    // - a restart button for failed and canceled transfers being sent
    // - a dismiss button for everything else
    if(state == TransferModel::Connecting || state == TransferModel::InProgress) {

        button->setText(tr("Cancel"));
        connect(button, &QPushButton::clicked, [this, index]() {
            mModel->cancel(index.row());
        });

    } else if(direction == TransferModel::Send && (state == TransferModel::Canceled || state == TransferModel::Failed)) {

        button->setText(tr("Restart"));
        connect(button, &QPushButton::clicked, [this, index]() {
            mModel->restart(index.row());
        });

    } else {

        button->setText(tr("Dismiss"));
        connect(button, &QPushButton::clicked, [this, index]() {
            mModel->dismiss(index.row());
        });
    }

    // Insert the button into the table
    ui->transferView->setIndexWidget(mModel->index(row, TransferModel::ActionColumn), button);
}
