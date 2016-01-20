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

#include <QObject>
#include <QProcessEnvironment>
#include <QStandardPaths>

#include "platform.h"

Platform::OperatingSystem Platform::currentOperatingSystem()
{
#if defined(Q_OS_WIN32)
    return OperatingSystem::Windows;
#elif defined(Q_OS_MACX)
    return OperatingSystem::OSX;
#elif defined(Q_OS_LINUX)
    return OperatingSystem::Linux;
#else
    return OperatingSystem::Unknown;
#endif
}

Platform::Architecture Platform::currentArchitecture()
{
    // Note: Qt doesn't provide the definitions we need to accurately
    // determine the CPU architecture - in order to do that, we'll need
    // to check a few preprocessor definitions ourselves

#if defined(__i386__) || defined(_M_IX86)
    return Architecture::x86;
#elif defined(__x86_64__) || defined(_M_X64)
    return Architecture::x64;
#else
    return Architecture::Unknown;
#endif
}

Platform::DesktopEnvironment Platform::currentDesktopEnvironment()
{
    QString desktop = QProcessEnvironment::systemEnvironment().value("XDG_CURRENT_DESKTOP").toLower();
    if(desktop == "unity") {
        return DesktopEnvironment::Unity;
    } else if(desktop.startsWith("gnome")) {
        return DesktopEnvironment::Gnome;
    } else if(desktop.endsWith("plasma")) {
        return DesktopEnvironment::KDE;
    } else if(desktop == "xfce") {
        return DesktopEnvironment::XFCE;
    } else if(desktop == "mate") {
        return DesktopEnvironment::MATE;
    } else if(desktop.endsWith("cinnamon")) {
        return DesktopEnvironment::Cinnamon;
    } else if(desktop == "pantheon") {
        return DesktopEnvironment::Pantheon;
    } else {
        return DesktopEnvironment::Unknown;
    }
}

QString Platform::operatingSystemName(OperatingSystem operatingSystem)
{
    switch(operatingSystem) {
    case OperatingSystem::Windows:
        return "windows";
    case OperatingSystem::OSX:
        return "osx";
    case OperatingSystem::Linux:
        return "linux";
    default:
        return "unknown";
    }
}

QString Platform::operatingSystemFriendlyName(OperatingSystem operatingSystem)
{
    switch(operatingSystem) {
    case OperatingSystem::Windows:
        return QObject::tr("Windows");
    case OperatingSystem::OSX:
        return QObject::tr("OS X");
    case OperatingSystem::Linux:
        return QObject::tr("Linux");
    default:
        return QObject::tr("Unknown");
    }
}

Platform::OperatingSystem Platform::operatingSystemForName(const QString &name)
{
    if(name == "windows") {
        return OperatingSystem::Windows;
    } else if(name == "osx") {
        return OperatingSystem::OSX;
    } else if(name == "linux") {
        return OperatingSystem::Linux;
    } else {
        return OperatingSystem::Unknown;
    }
}

QString Platform::architectureName(Architecture architecture)
{
    switch(architecture) {
    case Architecture::x86:
        return "x86";
    case Architecture::x64:
        return "x86_64";
    default:
        return "unknown";
    }
}

bool Platform::useIndicator()
{
    // Detecting the correct icon to display is incredibly difficult - the
    // algorithm currently being used goes something like this:
    //  - Windows: use QSystemTrayIcon
    //  - OS X: use QSystemTrayIcon
    //  - Linux:
    //     - Unity: use AppIndicator
    //     - Gnome: use AppIndicator
    //     - KDE: use QSystemTrayIcon
    //     - MATE: use AppIndicator
    //     - Cinnamon: use AppIndicator
    //     - Pantheon: use AppIndicator
    if(currentOperatingSystem() == OperatingSystem::Linux) {
        switch(currentDesktopEnvironment()) {
        case DesktopEnvironment::Unity:
        case DesktopEnvironment::Gnome:
        case DesktopEnvironment::MATE:
        case DesktopEnvironment::Cinnamon:
        case DesktopEnvironment::Pantheon:
            return true;
        }
    }

    // For everything else, use QSystemTrayIcon
    return false;
}
