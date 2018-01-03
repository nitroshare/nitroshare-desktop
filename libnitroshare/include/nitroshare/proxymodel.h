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

#ifndef LIBNITROSHARE_PROXYMODEL_H
#define LIBNITROSHARE_PROXYMODEL_H

#include <QAbstractProxyModel>

#include <nitroshare/config.h>

/**
 * @brief Proxy model with support for additional columns
 *
 * QIdentityProxyModel acts as a proxy for models provided by libnitroshare. It
 * can be used to add columns and customize appearance in a QTableView. The
 * class may not work with models from other sources.
 */
class NITROSHARE_EXPORT ProxyModel : public QAbstractProxyModel
{
    Q_OBJECT

public:

    /**
     * @brief Create a new proxy model
     * @param parent QObject
     */
    explicit ProxyModel(QObject *parent = nullptr);

    // Reimplemented virtual methods
    virtual void setSourceModel(QAbstractItemModel *sourceModel);
    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    virtual QModelIndex mapFromSource(const QModelIndex &sourceIndex) const;
    virtual QModelIndex mapToSource(const QModelIndex &proxyIndex) const;
    virtual QModelIndex parent(const QModelIndex &child) const;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

protected:

    QVariant sourceData(const QModelIndex &proxyIndex, int role) const;
};

#endif // LIBNITROSHARE_PROXYMODEL_H
