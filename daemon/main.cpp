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

#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDir>

#include <nitroshare/application.h>
#include <nitroshare/pluginmodel.h>

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    Application application;

    // Initialize the application
    app.setApplicationName("NitroShare");
    app.setApplicationVersion(application.version());
    app.setOrganizationName("Nathan Osman");
    app.setOrganizationDomain("nitroshare.net");

    // Create the parser and add the command line options
    QCommandLineParser parser;
    parser.setApplicationDescription("NitroShare");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption pluginDirOption(
        "plugin-dir",
        "Directory to load plugins from",
        "directory",
        QDir::currentPath() + QDir::separator() + "plugins"
    );
    parser.addOption(pluginDirOption);

    // Parse the command line
    if (!parser.parse(app.arguments())) {
        app.exit(1);
    }

    // Process the help option
    if (parser.isSet("help")) {
        parser.showHelp();
    }

    // Process the version option
    if (parser.isSet("version")) {
        parser.showVersion();
    }

    application.pluginModel()->addPlugins(parser.value(pluginDirOption));
    qDebug("%d plugins loaded", application.pluginModel()->rowCount(QModelIndex()));

    return app.exec();
}
