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

#include <QMetaObject>
#include <QMetaProperty>
#include <QVariantList>

#include <nitroshare/action.h>
#include <nitroshare/actionregistry.h>
#include <nitroshare/application.h>
#include <nitroshare/config.h>

#include "actionsaction.h"

ActionsAction::ActionsAction(Application *application)
    : mApplication(application)
{
}

QString ActionsAction::name() const
{
    return "actions";
}

bool ActionsAction::api() const
{
    return true;
}

QString ActionsAction::description() const
{
    return tr(
        "Retrieve a list of all registered actions."
        "This action takes no parameters and returns an array of action "
        "objects. Each action object includes details such as the action's "
        "name and description, which lists the expected parameters."
    );
}

QVariant ActionsAction::invoke(const QVariantMap &)
{
    // Create a list of the metadata for each action
    QVariantList actions;
    foreach (auto &action, mApplication->actionRegistry()->actions()) {

        // For each action, create a QVariantMap of its metadata
        QVariantMap properties;
        for (int i = 1; i < action->metaObject()->propertyCount(); ++i) {
            auto property = action->metaObject()->property(i);
            auto name = property.name();
            properties.insert(name, action->property(name));
        }

        // Add to the list
        actions.append(properties);
    }

    return actions;
}
