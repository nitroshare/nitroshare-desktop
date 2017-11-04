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

#include <nitroshare/application.h>
#include <nitroshare/device.h>
#include <nitroshare/transfer.h>
#include <nitroshare/transfermodel.h>
#include <nitroshare/transport.h>
#include <nitroshare/transportserver.h>

#include "transfermodel_p.h"

TransferModelPrivate::TransferModelPrivate(TransferModel *model, Application *application)
    : QObject(model),
      q(model),
      application(application)
{
}

TransferModelPrivate::~TransferModelPrivate()
{
    // TODO: stop transfers gracefully (?)

    qDeleteAll(transfers);
}

void TransferModelPrivate::sendDataChanged()
{
    int row = transfers.indexOf(qobject_cast<Transfer*>(sender()));
    emit q->dataChanged(q->index(row, 0), q->index(row, 0));
}

void TransferModelPrivate::processTransport(Transport *transport)
{
    // Create a new transfer from the transport
    q->addTransfer(new Transfer(application, transport));
}

TransferModel::TransferModel(Application *application, QObject *parent)
    : QAbstractListModel(parent),
      d(new TransferModelPrivate(this, application))
{
}

void TransferModel::addTransportServer(TransportServer *server)
{
    connect(server, &TransportServer::transportReceived, d, &TransferModelPrivate::processTransport);
    d->transportServers.insert(server->name(), server);
}

void TransferModel::removeTransportServer(TransportServer *server)
{
    disconnect(server, &TransportServer::transportReceived, d, &TransferModelPrivate::processTransport);
    d->transportServers.remove(server->name());
}

TransportServer *TransferModel::findTransportServer(const QString &name) const
{
    return d->transportServers.value(name);
}

void TransferModel::addTransfer(Transfer *transfer)
{
    connect(transfer, &Transfer::stateChanged, d, &TransferModelPrivate::sendDataChanged);
    connect(transfer, &Transfer::progressChanged, d, &TransferModelPrivate::sendDataChanged);
    connect(transfer, &Transfer::deviceNameChanged, d, &TransferModelPrivate::sendDataChanged);
    connect(transfer, &Transfer::errorChanged, d, &TransferModelPrivate::sendDataChanged);

    beginInsertRows(QModelIndex(), d->transfers.count(), d->transfers.count());
    d->transfers.append(transfer);
    endInsertRows();
}

void TransferModel::dismiss(int index)
{
    if (index >= 0 && index < d->transfers.count()) {
        Transfer *transfer = d->transfers.at(index);
        if (transfer->isFinished()) {
            beginRemoveRows(QModelIndex(), index, index);
            d->transfers.removeAt(index);
            endRemoveRows();
            delete transfer;
        }
    }
}

void TransferModel::dismissAll()
{
    for (int i = 0; i < d->transfers.count(); ++i) {
        dismiss(i);
    }
}

int TransferModel::rowCount(const QModelIndex &) const
{
    return d->transfers.count();
}

QVariant TransferModel::data(const QModelIndex &index, int role) const
{
    // Ensure the index points to a valid row
    if (!index.isValid() || index.row() < 0 ||
            index.row() >= d->transfers.count() || role != Qt::UserRole) {
        return QVariant();
    }
    return QVariant::fromValue(d->transfers.at(index.row()));
}
