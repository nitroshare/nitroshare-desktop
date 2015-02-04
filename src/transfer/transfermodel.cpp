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
#include <QIcon>
#include <QStyle>

#include "transfermodel.h"

int TransferModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : mTransfers.count();
}

int TransferModel::columnCount(const QModelIndex &parent) const
{
    // The model displays transfer direction / device name, transfer progress, and cancel button
    return parent.isValid() ? 0 : 3;
}

QVariant TransferModel::data(const QModelIndex &index, int role) const
{
    TransferPointer transfer(mTransfers.at(index.row()));

    switch(role) {
    case Qt::DisplayRole:
        switch(index.column()) {
        case 0: return transfer->deviceName();
        case 1: return transfer->progress();
        }
    case Qt::DecorationRole:
        if(index.column() == 0) {
            switch(transfer->direction()) {
            case Transfer::Send:
                return QVariant::fromValue(QApplication::style()->standardIcon(QStyle::SP_ArrowUp));
            case Transfer::Receive:
                return QVariant::fromValue(QApplication::style()->standardIcon(QStyle::SP_ArrowDown));
            }
        }
    case Qt::UserRole:
        return QVariant::fromValue(transfer);
    }

    return QVariant();
}

QVariant TransferModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch(section) {
        case 0: return tr("Device Name");
        case 1: return tr("Percentage");
        }
    }

    return QVariant();
}

void TransferModel::add(TransferPointer transfer)
{
    beginInsertRows(QModelIndex(), mTransfers.count(), mTransfers.count());
    mTransfers.append(transfer);
    endInsertRows();

    // Lambdas save us from an awkward dilemma - slots wouldn't have access to
    // the TransferPointer, only the Transfer* itself - but the lambdas do!
    connect(transfer.data(), &Transfer::statusChanged, [this, transfer]() {
        int index = mTransfers.indexOf(transfer);
        emit dataChanged(this->index(index, 0), this->index(index, 1));
    });
    connect(transfer.data(), &Transfer::finished, [this, transfer]() {
        int index = mTransfers.indexOf(transfer);

        beginRemoveRows(QModelIndex(), index, index);
        mTransfers.removeAt(index);
        endRemoveRows();
    });

    transfer->start();
}
