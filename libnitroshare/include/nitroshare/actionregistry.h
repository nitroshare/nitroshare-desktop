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

#ifndef LIBNITROSHARE_ACTIONREGISTRY_H
#define LIBNITROSHARE_ACTIONREGISTRY_H

#include <QList>
#include <QObject>

#include <nitroshare/config.h>

class Action;

class NITROSHARE_EXPORT ActionRegistryPrivate;

/**
 * @brief List of actions provided by plugins
 */
class NITROSHARE_EXPORT ActionRegistry : public QObject
{
    Q_OBJECT

public:

    /**
     * @brief Create a new action registry
     * @param parent QObject
     */
    explicit ActionRegistry(QObject *parent = nullptr);

    /**
     * @brief Add the specified action to the registry
     * @param action pointer to Action
     *
     * It is up to the caller to ensure that action is properly freed when no
     * longer needed.
     */
    void add(Action *action);

    /**
     * @brief Remove the specified action from the registry
     * @param action
     */
    void remove(Action *action);

    /**
     * @brief Find an action by name
     * @param name unique identifier for the action
     * @return pointer to Action or nullptr
     */
    Action *find(const QString &name) const;

    /**
     * @brief Retrieve a list of all registered actions
     * @return list of actions
     */
    QList<Action*> actions() const;

Q_SIGNALS:

    /**
     * @brief Indicate that an action was added
     * @param action pointer to Action
     */
    void actionAdded(Action *action);

    /**
     * @brief Indicate that an action was removed
     * @param action pointer to Action
     */
    void actionRemoved(Action *action);

private:

    ActionRegistryPrivate *const d;
};

#endif // LIBNITROSHARE_ACTIONREGISTRY_H
