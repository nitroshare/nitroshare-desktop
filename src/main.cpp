/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Nathan Osman
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
 **/

#include <QApplication>
#include <QLibraryInfo>
#include <QLocale>
#include <QMessageBox>
#include <QTranslator>

#include "application/application.h"
#include "application/splashdialog.h"
#include "settings/settings.h"
#include "util/platform.h"
#include "config.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(false);

    // Set up application properties
    app.setApplicationDisplayName(PROJECT_NAME);
    app.setApplicationName(PROJECT_NAME);
    app.setApplicationVersion(PROJECT_VERSION);
    app.setOrganizationDomain(PROJECT_DOMAIN);
    app.setOrganizationName(PROJECT_AUTHOR);

    // Set up translations for Qt
    QTranslator qtTranslator;
    qtTranslator.load(QString("qt_%1").arg(QLocale::system().name()),
            QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    app.installTranslator(&qtTranslator);

    // Set up translations for NitroShare
    QTranslator nsTranslator;
    nsTranslator.load(QString("nitroshare_%1").arg(QLocale::system().name()), ":/qm");
    app.installTranslator(&nsTranslator);

    // Check to see if the splash screen has been displayed yet
    bool appSplash = Settings::instance()->get(Settings::Key::ApplicationSplash).toBool();

    // If not, display it and remember that the user has seen it
    if(!appSplash) {
        SplashDialog().exec();
        Settings::instance()->set(Settings::Key::ApplicationSplash, true);
    }

#ifdef APPINDICATOR_FOUND
    // If the splash had not been seen and the user is running Gnome,
    // warn them that they need a special extension installed
    if(!appSplash && Platform::currentDesktopEnvironment() ==
            Platform::DesktopEnvironment::Gnome) {
        QMessageBox::about(nullptr, QObject::tr("Warning"), QObject::tr(
                "Some versions of Gnome do not support AppIndicators. This prevents "
                "NitroShare from displaying an indicator in the notification area. "
                "Please ensure you have this extension installed before continuing:"
                "<br><br><a href='%1'>%1</a>"
        ).arg("https://extensions.gnome.org/extension/615/appindicator-support/"));
    }
#endif

    // Create the tray icon that runs the application
    Application nitroshare;
    Q_UNUSED(nitroshare);

    return app.exec();
}
