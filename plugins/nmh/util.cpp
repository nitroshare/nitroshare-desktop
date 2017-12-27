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

#include <QtGlobal>

#ifdef Q_OS_WIN32
#  define NTDDI_VERSION NTDDI_VISTA
#  define _WIN32_WINNT _WIN32_WINNT_VISTA
#  include <Objbase.h>
#  include <Shlobj.h>
#endif

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QJsonDocument>

#ifdef Q_OS_WIN32
#  include <QSettings>
#endif

#include <nitroshare/fileutil.h>

#include "util.h"

bool Util::install(Browser browser)
{
    // It is assumed that the nitroshare-nmh executable is located in the same
    // directory as the host for this plugin
    QString nmhPath = QDir::cleanPath(
        QFileInfo(QCoreApplication::arguments().at(0)).absolutePath() +
        QDir::separator() + "nitroshare-nmh"
#ifdef Q_OS_WIN32
        ".exe"
#endif
    );

    // Generate the JSON for the manifest file
    QJsonObject object = extraJson(browser);
    object.insert("name", extensionId(browser));
    object.insert("description", "NitroShare");
    object.insert("path", nmhPath);
    object.insert("type", "stdio");

    // Ensure the path for the manifest file exists
    QString parentPath = manifestPath(browser);
    if (parentPath.isNull()) {
        return false;
    }
    if (!QDir(parentPath).mkpath(".")) {
        return false;
    }

    // Create the manifest file and write its contents
    QString manifestFilename = QDir(parentPath).absoluteFilePath(
        QString("%1.json").arg(extensionId(browser))
    );
    if (!FileUtil::createFile(manifestFilename, QJsonDocument(object).toJson())) {
        return false;
    }

#ifdef Q_OS_WIN32

    // Windows requires one additional step - the creation of a registry key
    QSettings settings(registryKey(browser), QSettings::NativeFormat);
    settings.setValue("Default", manifestFilename);

#endif

    return true;
}

QString Util::extensionId(Browser browser)
{
    switch (browser) {
    case Chrome:
        return "net.nitroshare.chrome";
    case Firefox:
        return "net.nitroshare.firefox";
    }
}

QString Util::manifestPath(Browser browser)
{
#if defined(Q_OS_WIN32)

    // Obtain the path to the AppData\Local directory
    PWSTR pszPath;
    HRESULT hr = SHGetKnownFolderPath(
        FOLDERID_LocalAppData,
        KF_FLAG_CREATE,
        NULL,
        &pszPath
    );
    if (FAILED(hr)) {
        return QString();
    }
    QDir appData(QString::fromUtf16(reinterpret_cast<const ushort*>(pszPath)));
    CoTaskMemFree(pszPath);

    // Use the folder for storing the JSON file
    return appData.absoluteFilePath("NitroShare");

#elif defined(Q_OS_MACX)

    switch (browser) {
    case Chrome:
        return "Library/Application Support/Google/Chrome/NativeMessagingHosts";
    case Firefox:
        return "Library/Application Support/Mozilla/NativeMessagingHosts";
    }

#elif defined(Q_OS_LINUX)

    switch (browser) {
    case Chrome:
        return ".config/google-chrome/NativeMessagingHosts";
    case Firefox:
        return ".mozilla/native-messaging-hosts";
    }

#else
    // Other platforms are not currently supported by the plugin
    return QString();
#endif
}

QJsonObject Util::extraJson(Browser browser)
{
    switch (browser) {
    case Chrome:
        return QJsonObject{
            { "allowed_origins", "chrome-extension://cljjpoinofmbdnbnpebolibochlfenag/" }
        };
    case Firefox:
        return QJsonObject{
            { "allowed_extensions", "nitroshare-firefox@nitroshare.net" }
        };
    }
}

#ifdef Q_OS_WIN32

QString Util::registryKey(Browser browser)
{
    switch (browser) {
    case Chrome:
        return "HKEY_CURRENT_USER\\Software\\Google\\Chrome\\NativeMessagingHosts\\net.nitroshare.chrome";
    case Firefox:
        return "HKEY_CURRENT_USER\\Software\\Mozilla\\NativeMessagingHosts\\net.nitroshare.firefox";
    }
}

#endif
