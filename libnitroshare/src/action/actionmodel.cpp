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

#include <nitroshare/action.h>
#include <nitroshare/actionmodel.h>

#include "actionmodel_p.h"

const QByteArray NameName = "name";
const QByteArray TextName = "text";
const QByteArray DisabledName = "enabled";

ActionModelPrivate::ActionModelPrivate(QObject *parent)
    : QObject(parent)
{
}

ActionModel::ActionModel(QObject *parent)
    : QAbstractListModel(parent),
      d(new ActionModelPrivate(this))
{
}

void ActionModel::addAction(Action *action)
{
    beginInsertRows(QModelIndex(), d->actions.count(), d->actions.count());
    d->actions.append(action);
    endInsertRows();
}

void ActionModel::removeAction(Action *action)
{
    int row = d->actions.indexOf(action);
    if (row != -1) {
        beginRemoveRows(QModelIndex(), row, row);
        d->actions.removeAt(row);
        endRemoveRows();
    }
}

int ActionModel::rowCount(const QModelIndex &parent) const
{
    return d->actions.count();
}

QVariant ActionModel::data(const QModelIndex &index, int role) const
{
    // Ensure the index points to a valid row
    if (!index.isValid() || index.row() < 0 || index.row() >= d->actions.count()) {
        return QVariant();
    }

    Action *action = d->actions.at(index.row());

    switch(role) {
    case NameRole:
        return action->property(Action::NameKey);
    case TextRole:
        return action->property(Action::TextKey);
    case DisabledRole:
        return action->property(Action::DisabledKey);
    }

    return QVariant();
}

QHash<int, QByteArray> ActionModel::roleNames() const
{
    return {
        { NameRole, NameName },
        { TextRole, TextName },
        { DisabledRole, DisabledName }
    };
}
