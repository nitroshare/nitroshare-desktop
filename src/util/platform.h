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

#ifndef NS_PLATFORM_H
#define NS_PLATFORM_H

#include <QString>

/**
 * @brief Platform information methods
 *
 * Some of the application functionality depends on platform-specific code.
 * Rather than depending on #define statements throughout the source code,
 * this class provides methods that can be invoked during runtime to determine
 * information about the current platform.
 *
 * It is important to note that the information provided by the first two
 * methods is determined during compile-time. Therefore, running an x86 build
 * of the application on 64-bit Windows will still cause currentArchitecture()
 * to indicate x86. This is considered expected behavior since it will cause
 * the update checker to retrieve updates for the same architecture.
 */
class Platform
{
public:

    /**
     * @brief Operating system definitions
     *
     * In order for the update checker to pull the correct update, we need to
     * split Linux distributions into those that are Debian-based and those
     * that are RPM-based. A generic "Linux" entry is provided as a catch-all.
     */
    enum class OperatingSystem : int {
        Unknown = 0,
        Windows,
        OSX,
        Debian,
        RPM,
        Linux
    };

    /**
     * @brief CPU architecture definitions
     */
    enum class Architecture : int {
        Unknown = 0,
        x86,
        x64
    };

    /**
     * @brief Desktop environment
     *
     * This is needed in order to determine the correct class to use for
     * showing the tray icon. In addition, some environments require special
     * add-ons to be installed (like Gnome).
     */
    enum class DesktopEnvironment : int {
        Unknown = 0,
        Explorer, // Windows
        Aqua,     // OS X
        Unity,    // Ubuntu
        Gnome,
        KDE,
        XFCE,
        MATE,
        Cinnamon, // Mint
        Pantheon  // Elementary
    };

    /**
     * @brief Retrieve the current operating system
     * @return one of OperatingSystem
     */
    static OperatingSystem currentOperatingSystem();

    /**
     * @brief Retrieve the current CPU architecture
     * @return one of Architecture
     */
    static Architecture currentArchitecture();

    /**
     * @brief Retrieve the current desktop environment
     * @return one of DesktopEnvironment
     */
    static DesktopEnvironment currentDesktopEnvironment();

    /**
     * @brief Retrieve the machine-friendly name of an OperatingSystem
     * @param operatingSystem one of OperatingSystem
     * @return operating system name
     *
     * If operatingSystem is not supplied, the name of the current operating
     * system is returned.
     */
    static QString operatingSystemName(OperatingSystem operatingSystem = currentOperatingSystem());

    /**
     * @brief Retrieve the human-friendly name of an OperatingSystem
     * @param operatingSystem one of OperatingSystem
     * @return operating system friendly name
     */
    static QString operatingSystemFriendlyName(OperatingSystem operatingSystem = currentOperatingSystem());

    /**
     * @brief Retrieve the machine-friendly name of an Architecture
     * @param architecture one of Architecture
     * @return architecture name
     */
    static QString architectureName(Architecture architecture = currentArchitecture());

    /**
     * @brief Retrieve the corresponding OperatingSystem for a name
     * @param name operating system name
     * @return one of OperatingSystem
     */
    static OperatingSystem operatingSystemForName(const QString &name);

    /**
     * @brief Determine if an OperatingSystem is Linux-based
     * @param operatingSystem one of OperatingSystem
     * @return
     */
    static bool isLinux(OperatingSystem operatingSystem = currentOperatingSystem());

    /**
     * @brief Determine if the current platform is Unity
     * @return true if running under Unity
     *
     * This method determines the current desktop environment during runtime.
     */
    static bool useIndicator();
};

#endif // NS_PLATFORM_H
