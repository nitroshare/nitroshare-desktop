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

#ifndef TRAYPLUGIN_H
#define TRAYPLUGIN_H

#include <QApplication>
#include <QMenu>
#include <QSystemTrayIcon>

#include <nitroshare/iplugin.h>

/**
 * @brief Provide a system tray icon
 */
class Q_DECL_EXPORT TrayPlugin : public IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID Plugin_iid)

public:

    virtual void initialize(Application *application);
    virtual void cleanup(Application *application);

private:

    QApplication *mApplication;
    QSystemTrayIcon *mIcon;
    QMenu *mMenu;
};

#endif // TRAYPLUGIN_H
