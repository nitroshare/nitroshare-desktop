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
#include <QPushButton>

#include "transferdelegate.h"
#include "transferwindow.h"
#include "ui_transferwindow.h"

TransferWindow::TransferWindow(TransferModel *model)
    : ui(new Ui::TransferWindow),
      mModel(model)
{
    ui->setupUi(this);

    ui->transferView->setModel(mModel);
    ui->transferView->setItemDelegate(new TransferDelegate(this));

    connect(ui->clear, &QPushButton::clicked, mModel, &TransferModel::clear);
    connect(mModel, &TransferModel::dataChanged, this, &TransferWindow::onDataChanged);
}

TransferWindow::~TransferWindow()
{
    delete ui;
}

void TransferWindow::onDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight,  const QVector<int> &roles)
{
    for(int row=topLeft.row(); row <= bottomRight.row(); ++row) {

        // If the state role changed, check to see what it was set to
        if(roles.contains(TransferModel::StateRole)) {
            switch(mModel->index(row, 0).data(TransferModel::StateRole).toInt()) {
            case TransferModel::Connecting:
            case TransferModel::InProgress:
                {
                    QPushButton * button = new QPushButton(tr("Cancel"));
                    QPersistentModelIndex index(mModel->index(row, 0));

                    // Cancel the transfer when the button is clicked
                    connect(button, &QPushButton::clicked, [this, index]() {
                        mModel->cancel(index.row());
                    });

                    ui->transferView->setIndexWidget(mModel->index(row, TransferModel::ActionColumn), button);
                }
                break;
            case TransferModel::Canceled:
            case TransferModel::Failed:
            case TransferModel::Succeeded:
                {
                    QPushButton * button = new QPushButton(tr("Restart"));
                    QPersistentModelIndex index(mModel->index(row, 0));

                    // Restart the transfer when the button is clicked
                    connect(button, &QPushButton::clicked, [this, index]() {
                        mModel->restart(index.row());
                    });

                    ui->transferView->setIndexWidget(mModel->index(row, TransferModel::ActionColumn), button);
                }
                break;
            }
        }
    }
}
