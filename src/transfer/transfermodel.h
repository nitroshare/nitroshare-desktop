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

#include "transfer.h"

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

public:

    /**
     * @brief Create a transfer model
     */
    TransferModel();

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
     * @brief Retrieve the index of the specified transfer
     * @param transfer transfer to retrieve the index of
     * @param column column to set the index to
     * @return index of the transfer
     */
    QModelIndex indexOf(Transfer *transfer, int column) const;

Q_SIGNALS:

    /**
     * @brief Indicate that a transfer has been added to the model
     * @param transfer transfer added
     *
     * The pointer is only guaranteed to exist for the duration of the slots
     * connected to this signal and should not be stored.
     */
    void transferAdded(Transfer *transfer);

    /**
     * @brief Indicate that a transfer has been removed from the model
     * @param transfer transfer removed
     *
     * The pointer is only guaranteed to exist for the duration of the slots
     * connected to this signal and should not be stored.
     */
    void transferRemoved(Transfer *transfer);

public Q_SLOTS:

    /**
     * @brief Add a transfer to the model
     * @param transfer transfer to add
     *
     * The model takes ownership of the transfer and starts it.
     */
    void add(Transfer *transfer);

    /**
     * @brief Remove all finished transfers from the model
     */
    void clear();

private:

    TransferModelPrivate *const d;
};

#endif // NS_TRANSFERMODEL_H
