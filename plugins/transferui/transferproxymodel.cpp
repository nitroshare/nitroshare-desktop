/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Nathan Osman
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

#include <QColor>

#include <nitroshare/transfer.h>

#include "transferproxymodel.h"

int TransferProxyModel::columnCount(const QModelIndex &) const
{
    return ColumnCount;
}

QVariant TransferProxyModel::data(const QModelIndex &proxyIndex, int role) const
{
    if (!proxyIndex.isValid() || proxyIndex.row() < 0 || proxyIndex.row() >= rowCount()) {
        return QVariant();
    }

    Transfer *transfer = sourceData(proxyIndex, Qt::UserRole).value<Transfer*>();

    switch (role) {
    case Qt::DisplayRole:
        switch (proxyIndex.column()) {
        case DeviceColumn:
            return transfer->deviceName();
        case ProgressColumn:
            return QString("%1%").arg(transfer->progress());
        case SpeedColumn:
            return formatSpeed(transfer->speed());
        case TimeRemainingColumn:
            return formatTimeRemaining(transfer->speed(), transfer->bytesRemaining());
        case StatusColumn:
            switch (transfer->state()) {
            case Transfer::Connecting:
                return tr("Connecting");
            case Transfer::InProgress:
                return tr("In Progress");
            case Transfer::Failed:
                return transfer->error();
            case Transfer::Succeeded:
                return tr("Succeeded");
            }
        }
        break;
    case Qt::ForegroundRole:
        if (proxyIndex.column() == StatusColumn) {
            switch (transfer->state()) {
            case Transfer::Failed:
                return QColor(Qt::darkRed);
            case Transfer::Succeeded:
                return QColor(Qt::darkGreen);
            default:
                break;
            }
        }
        break;
    case Qt::TextAlignmentRole:
        switch (proxyIndex.column()) {
        case ProgressColumn:
        case SpeedColumn:
        case TimeRemainingColumn:
            return Qt::AlignRight + Qt::AlignVCenter;
        }
        break;
    }

    return sourceData(proxyIndex, role);
}

QVariant TransferProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole) {
        return QVariant();
    }
    switch (section) {
    case DeviceColumn:
        return tr("Device");
    case ProgressColumn:
        return tr("Progress");
    case SpeedColumn:
        return tr("Speed");
    case TimeRemainingColumn:
        return tr("Time Remaining");
    case StatusColumn:
        return tr("Status");
    }
    return QVariant();
}

QString TransferProxyModel::formatSpeed(qint64 speed) const
{
    double unitSpeed = static_cast<double>(speed);
    QStringList units{ "B", "KB", "MB", "GB" };
    QStringList::const_iterator i;

    // Loop until the speed is less than 1000 of a unit (or no more units)
    for (i = units.constBegin(); i != units.constEnd(); ++i) {
        if (unitSpeed < 1e3) {
            break;
        }
        unitSpeed /= 1e3;
    }

    // Return the formatted speed
    return tr("%1 %2/s").arg(unitSpeed, 0, 'f', 1).arg(*i);
}

QString TransferProxyModel::formatTimeRemaining(qint64 speed, qint64 bytesRemaining) const
{
    // If the speed is unknown, avoid dividing by zero
    if (!speed) {
        return tr("unknown");
    }

    qint64 secondsRemaining = static_cast<qint64>(
        static_cast<double>(bytesRemaining) / static_cast<double>(speed)
    );

    // Calculate the number of hours, minutes, and seconds remaining
    qint64 hoursRemaining = secondsRemaining / 3600;
    qint64 minutesRemaining = secondsRemaining / 60 - hoursRemaining * 60;
    secondsRemaining -= hoursRemaining * 3600 + minutesRemaining * 60;

    // Return the formatted duration
    return tr("%1:%2:%3")
        .arg(hoursRemaining, 2, 10, QChar('0'))
        .arg(minutesRemaining, 2, 10, QChar('0'))
        .arg(secondsRemaining, 2, 10, QChar('0'));
}
