/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Nathan Osman
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

#ifndef LIBNITROSHARE_BUNDLE_H
#define LIBNITROSHARE_BUNDLE_H

#include <QAbstractListModel>

#include "config.h"

class Item;

class NITROSHARE_EXPORT BundlePrivate;

/**
 * @brief Bundle for transfer
 */
class NITROSHARE_EXPORT Bundle : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(qint64 totalSize READ totalSize)

public:

    enum Role {
        TypeRole = Qt::UserRole,
        NameRole,
        SizeRole
    };

    /**
     * @brief Create a new (empty) bundle
     * @param parent QObject
     */
    explicit Bundle(QObject *parent = nullptr);

    /**
     * @brief Add an item to the bundle
     * @param item to add
     *
     * The bundle assumes ownership of the item.
     */
    void addItem(Item *item);

    /**
     * @brief Total size of bundle contents
     * @return size in bytes
     */
    qint64 totalSize() const;

    // Reimplemented virtual methods
    virtual int rowCount(const QModelIndex &parent) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual QHash<int, QByteArray> roleNames() const;

private:

    BundlePrivate *const d;
};

#endif // LIBNITROSHARE_BUNDLE_H
