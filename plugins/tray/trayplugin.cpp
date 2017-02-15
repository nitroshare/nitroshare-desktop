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

#include <QAction>
#include <QIcon>
#include <QMenu>
#include <QMessageBox>
#include <QSystemTrayIcon>

#include <nitroshare/action.h>
#include <nitroshare/actionregistry.h>
#include <nitroshare/application.h>

#include "trayplugin.h"

void TrayPlugin::initialize(Application *application)
{
    mApplication = application;

    // Create the menu for the tray icon
    mMenu = new QMenu;

    // Create the separator
    mSeparator = new QAction(mMenu);
    mSeparator->setSeparator(true);
    mSeparator->setVisible(false);

    // Populate the menu
    mMenu->addAction(mSeparator);
    mMenu->addAction(tr("About"), this, SLOT(onAbout()));
    mMenu->addAction(tr("About Qt"), this, SLOT(onAboutQt()));
    mMenu->addSeparator();
    mMenu->addAction(tr("Quit"), mApplication, SLOT(quit()));

    // Create the tray icon and initialize it
    mIcon = new QSystemTrayIcon;
    mIcon->setContextMenu(mMenu);
    mIcon->setIcon(QIcon(":/icon/icon.png"));
    mIcon->show();

    // Add all existing actions to the menu
    foreach (Action *action, mApplication->actionRegistry()->actions()) {
        onActionAdded(action);
    }

    // Watch for new actions being added and removed
    connect(application->actionRegistry(), &ActionRegistry::actionAdded, this, &TrayPlugin::onActionAdded);
    connect(application->actionRegistry(), &ActionRegistry::actionRemoved, this, &TrayPlugin::onActionRemoved);
}

void TrayPlugin::cleanup(Application *)
{
    delete mIcon;
    delete mMenu;
}

void TrayPlugin::onActionAdded(Action *action)
{
    // Only actions with the "ui" property set are added
    if (!action->property("ui").toBool()) {
        return;
    }

    // Use the title property (if present) or fallback to the name
    QString title = action->property("title").toString();
    if (title.isEmpty()) {
        title = action->name();
    }

    // Create a menu item for the action and connect to its triggered() signal
    QAction *menuAction = new QAction(title, mMenu);
    menuAction->setData(QVariant::fromValue(action));
    connect(menuAction, &QAction::triggered, [action]() {
        action->invoke();
    });

    // If this is the first action, show the separator
    mSeparator->setVisible(true);

    // Insert the action
    mMenu->insertAction(mSeparator, menuAction);
}

void TrayPlugin::onActionRemoved(Action *action)
{
    //...
}

void TrayPlugin::onAbout()
{
    QMessageBox::information(
        nullptr,
        tr("About NitroShare"),
        tr("NitroShare - version %1\nCopyright 2017 Nathan Osman").arg(mApplication->version())
    );
}

void TrayPlugin::onAboutQt()
{
    QMessageBox::aboutQt(nullptr);
}
