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

#ifndef NS_TRANSFERMODEL_H
#define NS_TRANSFERMODEL_H

#include <QAbstractTableModel>
#include <QHostAddress>

#include "../bundle/bundle.h"

class TransferModelPrivate;

/**
 * @brief Data model for transfers
 *
 * Transfers are created in one of two ways: in response to an incoming TCP
 * request or as the result of a user directive to transfer files. Once
 * started, they remain in the model until cleared, allowing them to be
 * restarted in case of error.
 */
class TransferModel : public QAbstractTableModel
{
    Q_OBJECT
    Q_ENUMS(Direction)
    Q_ENUMS(State)

public:

    /**
     * @brief Columns displayed in model views
     */
    enum Column {
        /// Name of other device
        DeviceNameColumn = 0,
        /// Progress of transfer
        ProgressColumn,
        /// State of transfer
        StateColumn,
        /// Action that can be taken for the transfer
        ActionColumn,
        /// Total number of columns
        ColumnCount
    };

    /**
     * @brief Roles for retrieving data from the model
     */
    enum Role {
        /// Name of other device
        DeviceNameRole = Qt::UserRole,
        /// Progress of transfer
        ProgressRole,
        /// Direction of transfer
        DirectionRole,
        /// State of transfer
        StateRole,
        /// Error encountered during transfer
        ErrorRole
    };

    /**
     * @brief Direction of transfer
     */
    enum Direction {
        /// Transfer is sending files
        Send,
        /// Transfer is receiving files
        Receive
    };

    /**
     * @brief State of transfer
     */
    enum State {
        /// Connecting to remote host
        Connecting,
        /// File transfer in progress
        InProgress,
        /// Transfer failed
        Failed,
        /// Transfer succeeded
        Succeeded
    };

    /**
     * @brief Create a transfer model
     * @param parent parent QObject
     */
    explicit TransferModel(QObject *parent = nullptr);

    /**
     * @brief Destroy the transfer model
     */
    virtual ~TransferModel();

    /**
     * @brief Retrieve the number of rows in the model
     * @param parent parent index
     * @return number of rows
     */
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

    /**
     * @brief Retrieve the number of columns in the model
     * @param parent parent index
     * @return number of columns
     */
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;

    /**
     * @brief Retrieve data for the specified index
     * @param index index to retrieve
     * @param role role to retrieve
     * @return retrieved data
     */
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    /**
     * @brief Retrieve header data for the specified section
     * @param section section to retrieve
     * @param orientation orientation to retrieve
     * @param role role to retrieve
     * @return retrieved data
     */
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    /**
     * @brief Retrieve names of all roles
     * @return hash of role names
     */
    virtual QHash<int, QByteArray> roleNames() const;

    /**
     * @brief Calculate the combined progress of all transfers
     * @return progress of all transfers
     *
     * In order to avoid costly processing, the value is cached for a brief
     * period of time and all subsequent calls will return the cached value.
     */
    int combinedProgress() const;

public Q_SLOTS:

    /**
     * @brief Create a new transfer for receiving files
     * @param socketDescriptor socket descriptor
     * @param parent parent QObject
     */
    void addReceiver(qintptr socketDescriptor);

    /**
     * @brief Create a new transfer for sending files
     * @param deviceName name of the device
     * @param address address of device
     * @param port port of device
     * @param bundle bundle to send to the specified device
    */
    void addSender(const QString &deviceName, const QHostAddress &address, quint16 port, const Bundle *bundle);

    /**
     * @brief Cancel the specified transfer
     * @param index item index
     */
    void cancel(int index);

    /**
     * @brief Restart the specified transfer
     * @param index item index
     */
    void restart(int index);

    /**
     * @brief Dismiss the specified trasfer
     * @param index item index
     */
    void dismiss(int index);

    /**
     * @brief Remove all finished transfers from the model
     */
    void clear();

private:

    TransferModelPrivate *const d;
    friend class TransferModelPrivate;
};

#endif // NS_TRANSFERMODEL_H
