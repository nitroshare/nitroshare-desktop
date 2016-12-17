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

#ifndef LIBNITROSHARE_ACTIONMODEL_H
#define LIBNITROSHARE_ACTIONMODEL_H

#include <QAbstractListModel>

#include "config.h"

class Action;

class NITROSHARE_EXPORT ActionModelPrivate;

/**
 * @brief Model that represents actions that can be invoked
 *
 * In order to simplify invoking actions from other plugins, each action must
 * be registered with an instance of this class.
 */
class NITROSHARE_EXPORT ActionModel : public QAbstractListModel
{
    Q_OBJECT

public:

    /**
     * @brief Create a new model for Action instances
     * @param parent QObject
     */
    explicit ActionModel(QObject *parent = nullptr);

    /**
     * @brief Add an action to the model
     * @param action pointer to Action instance
     */
    void addAction(Action *action);

    /**
     * @brief Remove an action from the model
     * @param action pointer to Action instance
     */
    void removeAction(Action *action);

    // Reimplemented virtual methods
    virtual int rowCount(const QModelIndex &parent) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual QHash<int, QByteArray> roleNames() const;

private:

    ActionModelPrivate *const d;
};

#endif // LIBNITROSHARE_ACTIONMODEL_H
