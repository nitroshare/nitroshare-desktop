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

#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QCoreApplication>

#include <nitroshare/application.h>
#include <nitroshare/pluginregistry.h>
#include <nitroshare-common/init.h>

void init(Application *application)
{
    // Retrieve a pointer to the global application instance
    QCoreApplication *app = QCoreApplication::instance();

    // Initialize the application
    app->setApplicationName("NitroShare");
    app->setApplicationVersion(application->version());
    app->setOrganizationName("Nathan Osman");
    app->setOrganizationDomain("nitroshare.net");

    // Create the parser and add the command line options
    QCommandLineParser parser;
    parser.setApplicationDescription("NitroShare");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption pluginDirOption(
        "plugin-dir",
        "Directory to load plugins from",
        "directory"
    );
    parser.addOption(pluginDirOption);

    QCommandLineOption blacklistOption(
        "blacklist",
        "Plugin to blacklist from loading",
        "plugin"
    );
    parser.addOption(blacklistOption);

    // Parse the command line (exit on failure)
    parser.process(*app);

    // Apply the options
    application->pluginRegistry()->loadPluginsFromDirectories(
        parser.values(pluginDirOption)
    );
}
