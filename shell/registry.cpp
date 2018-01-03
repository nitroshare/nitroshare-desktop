/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Nathan Osman
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

#include "registry.h"

LONG setValue(HKEY hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, LPCTSTR lpData)
{
    HKEY hSubKey = NULL;
    LONG lRet;

    do {
        lRet = RegCreateKeyEx(
            hKey,
            lpSubKey,
            0,
            NULL,
            0,
            KEY_WRITE,
            NULL,
            &hSubKey,
            NULL
        );

        if (lRet != ERROR_SUCCESS) {
            break;
        }

        lRet = RegSetValueEx(
            hSubKey,
            lpValueName,
            0,
            REG_SZ,
            (LPBYTE) lpData,
            lstrlen(lpData) + 1
        );
    } while(false);

    if (hSubKey) {
        RegCloseKey(hSubKey);
    }

    return lRet;
}

LONG deleteKey(HKEY hKey, LPCTSTR lpSubKey)
{
    HKEY hSubKey;
    if (RegOpenKeyEx(hKey, lpSubKey, 0, KEY_READ, &hSubKey) == ERROR_SUCCESS) {
        RegCloseKey(hSubKey);
        return RegDeleteTree(hKey, lpSubKey);
    } else {
        return ERROR_SUCCESS;
    }
}

LONG deleteValue(HKEY hKey, LPCTSTR lpSubKey, LPCTSTR lpValue)
{
    if (RegGetValue(hKey, lpSubKey, lpValue, 0, NULL, NULL, NULL) == ERROR_SUCCESS) {
        return RegDeleteKeyValue(hKey, lpSubKey, lpValue);
    } else {
        return ERROR_SUCCESS;
    }
}
