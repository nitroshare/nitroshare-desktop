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

#ifndef LIBNITROSHARE_ACTION_H
#define LIBNITROSHARE_ACTION_H

#include <QObject>
#include <QVariantMap>

#include <nitroshare/config.h>

/**
 * @brief Procedure that can be invoked upon request
 *
 * Each individual action that can be invoked from a menu or from a script
 * must derive from this class. The invoke() slot is used to invoke the
 * action.
 */
class NITROSHARE_EXPORT Action : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name)

public:

    /**
     * @brief Retrieve the internal identifier for the action
     * @return action identifier
     */
    virtual QString name() const = 0;

public Q_SLOTS:

    /**
     * @brief Invoke the action
     * @param params parameters for the action
     * @return true if the action succeeded
     */
    virtual bool invoke(const QVariantMap &params = QVariantMap()) = 0;
};

#endif // LIBNITROSHARE_ACTION_H
