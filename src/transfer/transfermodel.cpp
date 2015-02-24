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
#include "transferreceiver.h"
#include "transfersender.h"

TransferModelPrivate::TransferModelPrivate(TransferModel *transferModel)
    : QObject(transferModel),
      q(transferModel)
{
}

TransferModelPrivate::~TransferModelPrivate()
{
    qDeleteAll(transfers);
}

void TransferModelPrivate::add(Transfer *transfer)
{
    q->beginInsertRows(QModelIndex(), transfers.count(), transfers.count());
    transfers.append(transfer);
    q->endInsertRows();

    // Whenever the transfer changes, emit the appropriate signal
    connect(transfer, &Transfer::dataChanged, [this, transfer]() {
        int index = transfers.indexOf(transfer);
        emit q->dataChanged(q->index(index, 0), q->index(index, TransferModel::ColumnCount - 1));
    });

    transfer->start();
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
    return parent.isValid() ? 0 : ColumnCount;
}

QVariant TransferModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid() || index.model() != this) {
        return QVariant();
    }

    Transfer *transfer = d->transfers.at(index.row());

    switch(role) {
    case Qt::DisplayRole:
        switch(index.column()) {
        case ColumnDeviceName:
            return transfer->deviceName();
        case ColumnProgress:
            return QString("%1%").arg(transfer->progress());
        case ColumnState:
            switch(transfer->state()) {
            case Connecting:
                return tr("Connecting");
            case InProgress:
                return tr("In Progress");
            case Canceled:
                return tr("Canceled");
            case Failed:
                return tr("Failed: %1").arg(transfer->error());
            case Succeeded:
                return tr("Succeeded");
            }
        }
        break;
    case Qt::DecorationRole:
        if(index.column() == ColumnDeviceName) {
            switch(transfer->direction()) {
            case Send:
                return QApplication::style()->standardIcon(QStyle::SP_ArrowUp);
            case Receive:
                return QApplication::style()->standardIcon(QStyle::SP_ArrowDown);
            }
        }
        break;
    case DeviceNameRole:
        return transfer->deviceName();
    case ProgressRole:
        return transfer->progress();
    case DirectionRole:
        return transfer->direction();
    case StateRole:
        return transfer->state();
    case ErrorRole:
        return transfer->error();
    }

    return QVariant();
}

QVariant TransferModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation != Qt::Horizontal || role != Qt::DisplayRole) {
        return QAbstractTableModel::headerData(section, orientation, role);
    }

    switch(section) {
    case ColumnDeviceName:
        return tr("Device Name");
    case ColumnProgress:
        return tr("Progress");
    case ColumnState:
        return tr("Status");
    }

    return QVariant();
}

QHash<int, QByteArray> TransferModel::roleNames() const
{
    return {
        {DeviceNameRole, "device_name"},
        {ProgressRole, "progress"},
        {DirectionRole, "direction"},
        {StateRole, "state"},
        {ErrorRole, "error"}
    };
}

void TransferModel::addReceiver(qintptr socketDescriptor)
{
    d->add(new TransferReceiver(socketDescriptor));
}

void TransferModel::addSender(const QString &deviceName, const QHostAddress &address, quint16 port, BundlePointer bundle)
{
    d->add(new TransferSender(deviceName, address, port, bundle));
}

void TransferModel::cancel(int index)
{
    if(index > 0 && index < d->transfers.count()) {
        d->transfers.at(index)->cancel();
    }
}

void TransferModel::restart(int index)
{
    if(index > 0 && index < d->transfers.count()) {
        d->transfers.at(index)->restart();
    }
}

void TransferModel::clear()
{
    // Iterate over the list in reverse to preserve indices when items are removed
    for(int i = d->transfers.count() - 1; i >= 0; --i) {
        Transfer *transfer = d->transfers.at(i);

        // Remove only items that are finished
        if(transfer->state() == Canceled || transfer->state() == Failed || transfer->state() == Succeeded) {
            beginRemoveRows(QModelIndex(), i, i);
            d->transfers.removeAt(i);
            endRemoveRows();

            delete transfer;
        }
    }
}
