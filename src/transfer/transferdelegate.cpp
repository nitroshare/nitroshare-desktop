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

#include <QApplication>
#include <QPainter>
#include <QStyleOptionProgressBar>

#include "transferdelegate.h"
#include "transfermodel.h"

// Padding between a cell border and its contents
const int CellPadding = 2;

TransferDelegate::TransferDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

void TransferDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                             const QModelIndex &index) const
{
    switch(index.column()) {
    case TransferModel::ProgressColumn:
        {
            int progress = index.data(TransferModel::ProgressRole).toInt();

            QStyleOptionProgressBar progressBar;
            progressBar.maximum = 100;
            progressBar.progress = progress;
            progressBar.rect = option.rect.adjusted(CellPadding, CellPadding, -CellPadding, -CellPadding);
            progressBar.text = QString("%1%").arg(progress);
            progressBar.textVisible = true;

            QApplication::style()->drawControl(QStyle::CE_ProgressBar, &progressBar, painter);
        }
        break;
    default:
        QStyledItemDelegate::paint(painter, option, index);
    }
}
