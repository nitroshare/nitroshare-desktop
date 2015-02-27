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

#include "platform.h"

Platform::OperatingSystem Platform::currentOperatingSystem()
{
#if defined(Q_OS_WIN32)
    return Windows;
#elif defined(Q_OS_MACX)
    return OSX;
#elif defined(Q_OS_LINUX)
    return Linux;
#else
    return Unknown;
#endif
}

Platform::Architecture Platform::currentArchitecture()
{
    // Note: Qt doesn't provide a define for 64-bit operating systems, so we
    // need to do this ourselves: __x86_64__ works on everything but MSVC++
#if defined(Q_OS_WIN64) || defined(__x86_64__)
    return x64;
#else
    return x86;
#endif
}

QString Platform::operatingSystemName(OperatingSystem operatingSystem)
{
    switch(operatingSystem) {
    case Windows:
        return "windows";
    case OSX:
        return "osx";
    case Linux:
        return "linux";
    default:
        return "unknown";
    }
}

QString Platform::operatingSystemFriendlyName(OperatingSystem operatingSystem)
{
    switch(operatingSystem) {
    case Windows:
        return QObject::tr("Windows");
    case OSX:
        return QObject::tr("OS X");
    case Linux:
        return QObject::tr("Linux");
    default:
        return QObject::tr("Unknown");
    }
}

Platform::OperatingSystem Platform::operatingSystemForName(const QString &name)
{
    if(name == "windows") {
        return Windows;
    } else if(name == "osx") {
        return OSX;
    } else if(name == "linux") {
        return Linux;
    } else {
        return Unknown;
    }
}

QString Platform::architectureName(Architecture architecture)
{
    switch(architecture) {
    case x86:
        return "x86";
    case x64:
        return "x64";
    default:
        return "unknown";
    }
}

bool Platform::isUnity()
{
    // Only check the environment variable when running under Linux
    if(currentOperatingSystem() == Linux) {
        return QProcessEnvironment::systemEnvironment().value("XDG_CURRENT_DESKTOP").toLower() == "unity";
    } else {
        return false;
    }
}
