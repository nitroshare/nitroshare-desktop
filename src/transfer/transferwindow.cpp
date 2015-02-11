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

#include <QIcon>
#include <QModelIndex>
#include <QPushButton>

#include "transferdelegate.h"
#include "transfermodel_p.h"
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
    connect(mModel, &TransferModel::transferAdded, this, &TransferWindow::add);
}

TransferWindow::~TransferWindow()
{
    delete ui;
}

void TransferWindow::add(Transfer *transfer)
{
    connect(transfer, &Transfer::stateChanged, [this, transfer](Transfer::State state) {
        QModelIndex index = mModel->indexOf(transfer, TransferModelPrivate::ColumnState);

        switch(state) {
        case Transfer::Connecting:
        case Transfer::InProgress:
        {
            QPushButton *button = new QPushButton(tr("Cancel"));
            connect(button, &QPushButton::clicked, transfer, &Transfer::cancel);

            ui->transferView->setIndexWidget(index, button);
            break;
        }
        case Transfer::Canceled:
        case Transfer::Failed:
        case Transfer::Succeeded:
        {
            QPushButton *button = new QPushButton(tr("Restart"));
            connect(button, &QPushButton::clicked, transfer, &Transfer::restart);

            ui->transferView->setIndexWidget(index, button);
            break;
        }
        default:
        {
            ui->transferView->setIndexWidget(index, nullptr);
        }
        }
    });
}
