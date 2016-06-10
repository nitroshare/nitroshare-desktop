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
#include <QBrush>
#include <QDateTime>
#include <QFile>
#include <QIcon>
#include <QSslCertificate>
#include <QSslKey>
#include <QStyle>

#include "transfermodel.h"
#include "transfermodel_p.h"
#include "transferreceiver.h"
#include "transfersender.h"

TransferModelPrivate::TransferModelPrivate(TransferModel *transferModel)
    : QObject(transferModel),
      q(transferModel),
      cachedProgress(0),
      cachedProgressAge(0)
{
    connect(Settings::instance(), &Settings::settingsChanged, this, &TransferModelPrivate::onSettingsChanged);

    configuration.setPeerVerifyMode(QSslSocket::VerifyPeer);
    configuration.setProtocol(QSsl::TlsV1_2OrLater);
    onSettingsChanged();
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
    QObject::connect(transfer, &Transfer::dataChanged, [this, transfer](const QVector<int> &roles) {
        if(!roles.contains(TransferModel::ProgressRole)) {
            cachedProgressAge = 0;
        }
        int index = transfers.indexOf(transfer);
        emit q->dataChanged(q->index(index, 0), q->index(index, TransferModel::ColumnCount - 1), roles);
    });

    transfer->start();
}

void TransferModelPrivate::remove(Transfer *transfer)
{
    // We need the index of the transfer for the removal signals
    int row = transfers.indexOf(transfer);

    emit q->beginRemoveRows(QModelIndex(), row, row);
    transfers.removeAt(row);
    emit q->endRemoveRows();

    delete transfer;
}

void TransferModelPrivate::onSettingsChanged(const QList<Settings::Key> &keys)
{
    Settings *settings = Settings::instance();

    // Load the CA certificate
    if (keys.empty() || keys.contains(Settings::Key::TLSCACertificate)) {
        QString filename = settings->get(Settings::Key::TLSCACertificate).toString();
        if (!filename.isEmpty()) {
            QFile file(filename);
            if (file.open(QIODevice::ReadOnly)) {
                QSslCertificate cert(&file, QSsl::Pem);
                if (!cert.isNull()) {
                    configuration.setCaCertificates(QList<QSslCertificate>({cert}));
                }
            }
        }
    }

    // Load the client certificate
    if (keys.empty() || keys.contains(Settings::Key::TLSCertificate)) {
        QString filename = settings->get(Settings::Key::TLSCertificate).toString();
        if (!filename.isEmpty()) {
            QFile file(filename);
            if (file.open(QIODevice::ReadOnly)) {
                QSslCertificate cert(&file, QSsl::Pem);
                if (!cert.isNull()) {
                    configuration.setLocalCertificate(cert);
                }
            }
        }
    }

    // Load the private key and passphrase
    if (keys.empty() || keys.contains(Settings::Key::TLSPrivateKey) ||
            keys.contains(Settings::Key::TLSPrivateKeyPassphrase)) {
        QString filename = settings->get(Settings::Key::TLSPrivateKey).toString();
        if (!filename.isEmpty()) {
            QFile file(filename);
            if (file.open(QIODevice::ReadOnly)) {
                QSslKey key(&file, QSsl::Rsa, QSsl::Pem, QSsl::PrivateKey,
                        settings->get(Settings::Key::TLSPrivateKeyPassphrase).toByteArray());
                if (!key.isNull()) {
                    configuration.setPrivateKey(key);
                }
            }
        }
    }
}

TransferModel::TransferModel(QObject *parent)
    : QAbstractTableModel(parent),
      d(new TransferModelPrivate(this))
{
}

TransferModel::~TransferModel()
{
    delete d;
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
        case DeviceNameColumn:
            return transfer->deviceName();
        case ProgressColumn:
            return QString("%1%").arg(transfer->progress());
        case StateColumn:
            switch(transfer->state()) {
            case Connecting:
                return tr("Connecting");
            case InProgress:
                return tr("In Progress");
            case Failed:
                return tr("Failed: %1").arg(transfer->error());
            case Succeeded:
                return tr("Succeeded");
            }
        }
        break;
    case Qt::DecorationRole:
        if(index.column() == DeviceNameColumn) {
            switch(transfer->direction()) {
            case Send:
                return QApplication::style()->standardIcon(QStyle::SP_ArrowUp);
            case Receive:
                return QApplication::style()->standardIcon(QStyle::SP_ArrowDown);
            }
        }
        break;
    case Qt::ForegroundRole:
        if(index.column() == StateColumn) {
            switch(transfer->state()) {
            case Failed:
                return QBrush(Qt::darkRed);
            case Succeeded:
                return QBrush(Qt::darkGreen);
            default:
                break;
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
    case DeviceNameColumn:
        return tr("Device Name");
    case ProgressColumn:
        return tr("Progress");
    case StateColumn:
        return tr("Status");
    case ActionColumn:
        return tr("Action");
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

int TransferModel::combinedProgress() const
{
    // If we're still within 200ms of the last call, return the cached value
    qint64 currentMSecs = QDateTime::currentMSecsSinceEpoch();
    if(d->cachedProgressAge + 200 > currentMSecs) {
        return d->cachedProgress;
    }

    // Sum the progress of all transfers in progress
    int progress = 0;
    int progressCount = 0;
    for(QList<Transfer*>::const_iterator i = d->transfers.constBegin(); i != d->transfers.constEnd(); ++i) {
        if((*i)->state() == InProgress) {
            progress += (*i)->progress();
            progressCount++;
        }
    }
    if(progressCount) {
        progress /= progressCount;
    }

    // Store the value for next time
    d->cachedProgressAge = currentMSecs;
    return d->cachedProgress = progress;
}

void TransferModel::addReceiver(qintptr socketDescriptor)
{
    d->add(new TransferReceiver(socketDescriptor));
}

void TransferModel::addSender(const QString &deviceName, const QHostAddress &address, quint16 port, const Bundle *bundle)
{
    d->add(new TransferSender(deviceName, address, port, bundle));
}

void TransferModel::cancel(int index)
{
    if(index < 0 || index >= d->transfers.count()) {
        qWarning("Invalid index supplied.");
        return;
    }

    d->transfers.at(index)->cancel();
}

void TransferModel::restart(int index)
{
    if(index < 0 || index >= d->transfers.count()) {
        qWarning("Invalid index supplied.");
        return;
    }

    d->transfers.at(index)->restart();
}

void TransferModel::dismiss(int index)
{
    if(index < 0 || index >= d->transfers.count()) {
        qWarning("Invalid index supplied.");
        return;
    }

    // Retrieve the transfer and ensure it is not in progress
    Transfer *transfer = d->transfers.at(index);
    if(transfer->state() == TransferModel::Connecting || transfer->state() == TransferModel::InProgress) {
        qWarning("Cannot dismiss a transfer that is currently in progress.");
        return;
    }

    d->remove(transfer);
}

void TransferModel::clear()
{
    // Iterate over the list in reverse to preserve indices when items are removed
    for(int i = d->transfers.count() - 1; i >= 0; --i) {
        Transfer *transfer = d->transfers.at(i);

        // Remove only items that are finished
        if(transfer->state() == Failed || transfer->state() == Succeeded) {
            d->remove(transfer);
        }
    }
}
