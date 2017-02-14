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

#include <QIcon>
#include <QMessageBox>

#include <nitroshare/application.h>

#include "trayplugin.h"

void TrayPlugin::initialize(Application *application)
{
    mMenu = new QMenu;
    mMenu->addAction(tr("About Qt"), this, SLOT(onAboutQt()));
    mMenu->addSeparator();
    mMenu->addAction(tr("Quit"), application, SLOT(quit()));

    mIcon = new QSystemTrayIcon;
    mIcon->setContextMenu(mMenu);
    mIcon->setIcon(QIcon(":/icon/icon.png"));
    mIcon->show();
}

void TrayPlugin::cleanup(Application *)
{
    delete mIcon;
    delete mMenu;
}

void TrayPlugin::onAboutQt()
{
    QMessageBox::aboutQt(nullptr);
}
