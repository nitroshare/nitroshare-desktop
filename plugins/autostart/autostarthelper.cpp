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

#include <QCoreApplication>
#include <QDir>

#if defined(Q_OS_WIN32)
#  include <QSettings>
#endif

#if defined(Q_OS_MACX) || defined(Q_OS_LINUX)
#  include <QFile>
#endif

#include <nitroshare/application.h>
#include <nitroshare/fileutil.h>
#include <nitroshare/settingsregistry.h>

#include "autostarthelper.h"

const QString Autostart = "Autostart";

#if defined(Q_OS_WIN32)
const QString RegistryKey = "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run";
#endif

#if defined(Q_OS_MACX)
const QString AutostartPath = "Library/LaunchAgents/com.NathanOsman.NitroShare.plist";
const QString AutostartTemplate =
    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
    "<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n"
    "<plist version=\"1.0\">\n"
    "<dict>\n"
    "    <key>Label</key>\n"
    "    <string>com.NathanOsman.NitroShare</string>\n"
    "    <key>ProgramArguments</key>\n"
    "    <array>\n"
    "        <string>%1</string>\n"
    "    </array>\n"
    "    <key>RunAtLoad</key>\n"
    "    <true/>\n"
    "</dict>\n"
    "</plist>\n";
#endif

#if defined(Q_OS_LINUX)
const QString AutostartPath = ".config/autostart/nitroshare.desktop";
const QString AutostartTemplate =
    "[Desktop Entry]\n"
    "Version=1.0\n"
    "Name=NitroShare\n"
    "Type=Application\n"
    "Exec=%1\n"
    "Terminal=false\n";
#endif

AutostartHelper::AutostartHelper(Application *application)
    : mApplication(application),
      mAutostart({
          { Setting::TypeKey, Setting::Boolean },
          { Setting::NameKey, Autostart },
          { Setting::TitleKey, tr("Start NitroShare after login") }
      })
{
    connect(mApplication->settingsRegistry(), &SettingsRegistry::settingsChanged, this, &AutostartHelper::onSettingsChanged);

    mApplication->settingsRegistry()->add(&mAutostart);
}

AutostartHelper::~AutostartHelper()
{
    mApplication->settingsRegistry()->remove(&mAutostart);
}

void AutostartHelper::onSettingsChanged(const QStringList &keys)
{
    if (keys.contains(Autostart)) {
        if (mApplication->settingsRegistry()->value(Autostart).toBool()) {
            enableAutostart();
        } else {
            disableAutostart();
        }
    }
}

QString AutostartHelper::pathToSelf() const
{
    return QDir::current().absoluteFilePath(QCoreApplication::arguments().at(0));
}

// TODO: these have no error handling

void AutostartHelper::enableAutostart()
{
#if defined(Q_OS_WIN32)
    QSettings settings(RegistryKey, QSettings::NativeFormat);
    settings.setValue("NitroShare", pathToSelf());
#elif defined(Q_OS_MACX) || defined(Q_OS_LINUX)
    FileUtil::createFile(
        QDir::home().absoluteFilePath(AutostartPath),
        AutostartTemplate.arg(pathToSelf()).toUtf8()
    );
#endif
}

void AutostartHelper::disableAutostart()
{
#if defined(Q_OS_WIN32)
    QSettings settings(RegistryKey, QSettings::NativeFormat);
    settings.remove("NitroShare");
#elif defined(Q_OS_MACX) || defined(Q_OS_LINUX)
    QFile(QDir::home().absoluteFilePath(AutostartPath)).remove();
#endif
}
