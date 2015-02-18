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
#include "transfermodel_p.h"

TransferModelPrivate::TransferModelPrivate(QObject *parent)
    : QObject(parent)
{
}

TransferModelPrivate::~TransferModelPrivate()
{
    qDeleteAll(transfers);
}

TransferModel::TransferModel()
    : d(new TransferModelPrivate(this))
{
}

int TransferModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : d->transfers.count();
}

int TransferModel::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : TransferModelPrivate::ColumnCount;
}

QVariant TransferModel::data(const QModelIndex &index, int role) const
{
    if(index.row() < d->transfers.count() && index.column() < TransferModelPrivate::ColumnCount) {
        Transfer *transfer = d->transfers.at(index.row());

        switch(role) {
        case Qt::DisplayRole:
            switch(index.column()) {
            case TransferModelPrivate::ColumnDeviceName:
                return transfer->deviceName();
            case TransferModelPrivate::ColumnProgress:
                return transfer->progress();
            case TransferModelPrivate::ColumnState:
                return transfer->state();
            }
        case Qt::DecorationRole:
            if(index.column() == TransferModelPrivate::ColumnDeviceName) {
                switch(transfer->direction()) {
                case Transfer::Send:
                    return QVariant::fromValue(QApplication::style()->standardIcon(QStyle::SP_ArrowUp));
                case Transfer::Receive:
                    return QVariant::fromValue(QApplication::style()->standardIcon(QStyle::SP_ArrowDown));
                }
            }
            break;
        case Qt::UserRole:
            return QVariant::fromValue(transfer);
        }
    }

    return QVariant();
}

QVariant TransferModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch(section) {
        case TransferModelPrivate::ColumnDeviceName:
            return tr("Device Name");
        case TransferModelPrivate::ColumnProgress:
            return tr("Progress");
        case TransferModelPrivate::ColumnState:
            return tr("Status");
        }
    }

    return QVariant();
}

QModelIndex TransferModel::indexOf(Transfer *transfer, int column) const
{
    return index(d->transfers.indexOf(transfer), column);
}

void TransferModel::add(Transfer *transfer)
{
    // Whenever properties of the transfer change, emit the appropriate signal
    connect(transfer, &Transfer::deviceNameChanged, [this, transfer]() {
        QModelIndex index = indexOf(transfer, TransferModelPrivate::ColumnDeviceName);
        emit dataChanged(index, index);
    });

    connect(transfer, &Transfer::progressChanged, [this, transfer]() {
        QModelIndex index = indexOf(transfer, TransferModelPrivate::ColumnProgress);
        emit dataChanged(index, index);
    });

    connect(transfer, &Transfer::stateChanged, [this, transfer]() {
        QModelIndex index = indexOf(transfer, TransferModelPrivate::ColumnState);
        emit dataChanged(index, index);
    });

    beginInsertRows(QModelIndex(), d->transfers.count(), d->transfers.count());
    d->transfers.append(transfer);
    endInsertRows();

    // This needs to come after inserting the transfer so that it will have a valid index
    emit transferAdded(transfer);
}

void TransferModel::clear()
{
    // Iterate over the list in reverse to preserve indices when items are removed
    for(int i = d->transfers.count() - 1; i >= 0; --i) {
        Transfer *transfer = d->transfers.at(i);

        if(transfer->state() == Transfer::Canceled || transfer->state() == Transfer::Failed ||
                transfer->state() == Transfer::Succeeded) {
            beginRemoveRows(QModelIndex(), i, i);
            d->transfers.removeAt(i);
            endRemoveRows();

            emit transferRemoved(transfer);
            delete transfer;
        }
    }
}
