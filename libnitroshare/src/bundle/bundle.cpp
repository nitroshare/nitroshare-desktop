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

#include <nitroshare/bundle.h>
#include <nitroshare/item.h>

#include "bundle_p.h"

const QByteArray TypeName = "type";
const QByteArray NameName = "name";
const QByteArray SizeName = "size";

BundlePrivate::BundlePrivate(QObject *parent)
    : QObject(parent),
      totalSize(0)
{
}

BundlePrivate::~BundlePrivate()
{
    qDeleteAll(items);
}

Bundle::Bundle(QObject *parent)
    : QAbstractListModel(parent),
      d(new BundlePrivate(this))
{
}

void Bundle::addItem(Item *item)
{
    d->items.append(item);
    d->totalSize += item->properties().value(Item::SizeKey).toLongLong();
}

qint64 Bundle::totalSize() const
{
    return d->totalSize;
}

int Bundle::rowCount(const QModelIndex &parent) const
{
    return d->items.count();
}

QVariant Bundle::data(const QModelIndex &index, int role) const
{
    // Ensure the index points to a valid row
    if (!index.isValid() || index.row() < 0 || index.row() >= d->items.count()) {
        return QVariant();
    }

    Item *item = d->items.at(index.row());

    switch (role) {
    case TypeRole:
        return item->properties().value(Item::TypeKey);
    case NameRole:
        return item->properties().value(Item::NameKey);
    case SizeRole:
        return item->properties().value(Item::SizeKey);
    }

    return QVariant();
}

QHash<int, QByteArray> Bundle::roleNames() const
{
    return {
        { TypeRole, TypeName },
        { NameRole, NameName },
        { SizeRole, SizeName }
    };
}
