/*
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

#if defined(Q_OS_UNIX)
#  include <sys/stat.h>
#elif defined(Q_OS_WIN32)
#  include <aclapi.h>
#  include <windows.h>
#endif

#include <qhttpengine/localfile.h>

#include "localfile_p.h"

using namespace QHttpEngine;

LocalFilePrivate::LocalFilePrivate(LocalFile *localFile)
    : QObject(localFile),
      q(localFile)
{
    // Store the file in the user's home directory and set the filename to the
    // name of the application with a "." prepended
    q->setFileName(QDir::home().absoluteFilePath("." + QCoreApplication::applicationName()));
}

bool LocalFilePrivate::setPermission()
{
#if defined(Q_OS_UNIX)
    return chmod(q->fileName().toUtf8().constData(), S_IRUSR | S_IWUSR) == 0;
#elif defined(Q_OS_WIN32)
    // Windows uses ACLs to control file access - each file contains an ACL
    // which consists of one or more ACEs (access control entries) - so the
    // ACL for the file must contain only a single ACE, granting access to the
    // file owner (the current user)

    EXPLICIT_ACCESS_W ea;
    ZeroMemory(&ea, sizeof(EXPLICIT_ACCESS_W));
    ea.grfAccessPermissions = GENERIC_ALL;
    ea.grfAccessMode = GRANT_ACCESS;
    ea.grfInheritance = SUB_CONTAINERS_AND_OBJECTS_INHERIT;
    ea.Trustee.TrusteeForm = TRUSTEE_IS_NAME;
    ea.Trustee.ptstrName = L"CURRENT_USER";

    // Create a new ACL with a single access control entry
    PACL pACL;
    if (SetEntriesInAclW(1, &ea, NULL, &pACL) != ERROR_SUCCESS) {
        return false;
    }

    // Apply the ACL to the file
    if (SetNamedSecurityInfoW((LPWSTR)q->fileName().utf16(),
                             SE_FILE_OBJECT,
                             DACL_SECURITY_INFORMATION | PROTECTED_DACL_SECURITY_INFORMATION,
                             NULL,
                             NULL,
                             pACL,
                             NULL) != ERROR_SUCCESS) {
        LocalFree(pACL);
        return false;
    }

    LocalFree(pACL);
    return true;
#else
    // Unsupported platform, so setPermission() must fail
    return false;
#endif
}

bool LocalFilePrivate::setHidden()
{
#if defined(Q_OS_UNIX)
    // On Unix, anything beginning with a "." is hidden
    return true;
#elif defined(Q_OS_WIN32)
    return SetFileAttributesW((LPCWSTR)q->fileName().utf16(), FILE_ATTRIBUTE_HIDDEN) != 0;
#else
    // Unsupported platform, so setHidden() must fail
    return false;
#endif
}

LocalFile::LocalFile(QObject *parent)
    : QFile(parent),
      d(new LocalFilePrivate(this))
{
}

bool LocalFile::open()
{
    return QFile::open(QIODevice::WriteOnly) && d->setPermission() && d->setHidden();
}
