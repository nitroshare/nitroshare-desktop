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

#if defined(Q_OS_WIN32)
#  define NTDDI_VERSION NTDDI_VISTA
#  include <Objbase.h>
#  include <Shlobj.h>
#endif

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QString>

#if defined(Q_OS_WIN32)
#  include <QSettings>
#endif

#include <nitroshare/fileutil.h>

#include "util.h"

#if defined(Q_OS_WIN32)
const QString RegistryKey = "HKEY_CURRENT_USER\\Software\\Google\\Chrome\\NativeMessagingHosts\\net.nitroshare.chrome";
#endif

const QString JsonTemplate =
    "{\n"
    "    \"name\": \"net.nitroshare.chrome\",\n"
    "    \"description\": \"NitroShare\",\n"
    "    \"path\": \"%1\",\n"
    "    \"type\": \"stdio\",\n"
    "    \"allowed_origins\": [\n"
    "        \"chrome-extension://cljjpoinofmbdnbnpebolibochlfenag/\"\n"
    "    ]\n"
    "}\n";

// TODO: add support for Chromium

bool Util::installJson()
{
    QString parentPath;

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
        return false;
    }
    QDir appData(QString::fromUtf16(reinterpret_cast<const ushort*>(pszPath)));
    CoTaskMemFree(pszPath);

    // Use the folder for storing the JSON file
    parentPath = appData.absoluteFilePath("NitroShare");

#elif defined(Q_OS_MACX)
    parentPath = QDir::home().absoluteFilePath("Library/Application Support/Google/Chrome/NativeMessagingHosts");
#elif defined(Q_OS_LINUX)
    parentPath = QDir::home().absoluteFilePath(".config/google-chrome/NativeMessagingHosts");
#else
    // Unknown platform; fail automatically
    return false;
#endif

    // Ensure that the parent path exists
    if (!QDir(parentPath).mkpath(".")) {
        return false;
    }

    // It is assumed that the nitroshare-nmh executable is located in the same
    // directory as the host for this plugin
    QString nmhPath = QDir::cleanPath(
        QFileInfo(QCoreApplication::arguments().at(0)).absolutePath() +
        QDir::separator() + "nitroshare-nmh"
#if defined(Q_OS_WIN32)
        + ".exe"
#endif
    );

    // Create the JSON file
    QString jsonFilename = QDir(parentPath).absoluteFilePath("net.nitroshare.chrome.json");
    if (!FileUtil::createFile(jsonFilename, JsonTemplate.arg(nmhPath).toUtf8())) {
        return false;
    }

#if defined(Q_OS_WIN32)

    // Windows requires one additional step - a registry entry must be created
    QSettings settings(RegistryKey, QSettings::NativeFormat);
    settings.setValue("Default", jsonFilename);

#endif

    // Everything succeeded
    return true;
}
