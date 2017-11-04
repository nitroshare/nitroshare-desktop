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

#include <QDate>
#include <QIcon>
#include <QMessageBox>

#include <nitroshare/action.h>
#include <nitroshare/actionregistry.h>
#include <nitroshare/application.h>

#include "traymenu.h"

TrayMenu::TrayMenu(Application *application)
    : mApplication(application),
      mSeparator(nullptr)
{
    connect(application->actionRegistry(), &ActionRegistry::actionAdded, this, &TrayMenu::onActionAdded);
    connect(application->actionRegistry(), &ActionRegistry::actionRemoved, this, &TrayMenu::onActionRemoved);

    mSeparator.setSeparator(true);
    mSeparator.setVisible(false);

    // TODO: these can be converted to the new connection syntax after support
    // for Qt 5.6 becomes a build requirement

    mMenu.addAction(&mSeparator);
    mMenu.addAction(tr("About..."), this, SLOT(onAbout()));
    mMenu.addAction(tr("About Qt..."), this, SLOT(onAboutQt()));
    mMenu.addSeparator();
    mMenu.addAction(tr("Quit"), mApplication, SLOT(quit()));

    // Add existing actions to the menu
    foreach (Action *action, application->actionRegistry()->actions()) {
        onActionAdded(action);
    }

    mIcon.setContextMenu(&mMenu);
    mIcon.setIcon(QIcon(":/tray/icon.png"));
    mIcon.show();
}

TrayMenu::~TrayMenu()
{
    qDeleteAll(mActions);
}

void TrayMenu::onActionAdded(Action *action)
{
    // Only add actions with the "ui" property set to true
    if (!action->property("ui").toBool()) {
        return;
    }

    // Use the "title" property if present or fallback to the name
    QString title = action->property("title").toString();
    if (title.isNull()) {
        title = action->name();
    }

    // Create a QAction for the action
    QAction *menuAction = new QAction(title, nullptr);
    connect(menuAction, &QAction::triggered, [action]() {
        action->invoke();
    });

    // Ensure the separator is shown
    mSeparator.setVisible(true);

    // Insert the action into the menu and store it in the hash
    mMenu.insertAction(&mSeparator, menuAction);
    mActions.insert(action->name(), menuAction);
}

void TrayMenu::onActionRemoved(Action *action)
{
    // Attempt to find the corresponding QAction
    QAction *menuAction = mActions.take(action->name());
    if (!menuAction) {
        return;
    }

    // Remove the action from the menu and hide the separator if nothing remains
    mMenu.removeAction(menuAction);
    if (!mActions.count()) {
        mSeparator.setVisible(false);
    }

    // Free the action
    delete menuAction;
}

void TrayMenu::onAbout()
{
    QMessageBox::information(
        nullptr,
        tr("About NitroShare"),
        tr("NitroShare - version %1\nCopyright %2 Nathan Osman")
            .arg(mApplication->version())
            .arg(QDate::currentDate().year())
    );
}

void TrayMenu::onAboutQt()
{
    QMessageBox::aboutQt(nullptr);
}
