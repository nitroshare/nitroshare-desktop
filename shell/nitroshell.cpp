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
 **/

#include <objbase.h>
#include <OleCtl.h>
#include <Shlobj.h>
#include <windows.h>

#include "classfactory.h"
#include "nitroshell.h"
#include "registry.h"

// {52A10783-C811-4C45-9A3D-221A962C8640}
static const GUID CLSID_NitroShellExt = { 0x52a10783, 0xc811, 0x4c45, { 0x9a, 0x3d, 0x22, 0x1a, 0x96, 0x2c, 0x86, 0x40 } };

HINSTANCE gInstance;
UINT gLockCount = 0;

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    if (fdwReason == DLL_PROCESS_ATTACH) {
        gInstance = hinstDLL;
    }

    return TRUE;
}

STDAPI DllCanUnloadNow()
{
    return gLockCount ? S_FALSE : S_OK;
}

STDAPI DllGetClassObject(REFIID rclsid, REFIID riid, LPVOID *ppv)
{
    if (!ppv) {
        return E_INVALIDARG;
    }

    *ppv = NULL;

    if (!IsEqualCLSID(rclsid, CLSID_NitroShellExt)) {
        return CLASS_E_CLASSNOTAVAILABLE;
    }

    ClassFactory *classFactory = new ClassFactory;
    HRESULT hResult = classFactory->QueryInterface(riid, ppv);
    classFactory->Release();
    return hResult;
}

STDAPI DllRegisterServer()
{
    TCHAR filename[MAX_PATH];
    if (!GetModuleFileName(gInstance, filename, MAX_PATH)) {
        return SELFREG_E_CLASS;
    }

    if (setValue(
            HKEY_LOCAL_MACHINE,
            TEXT("Software\\Classes\\CLSID\\{52A10783-C811-4C45-9A3D-221A962C8640}"),
            NULL,
            TEXT("NitroShellExt")) != ERROR_SUCCESS) {
        return SELFREG_E_CLASS;
    }

    if (setValue(
            HKEY_LOCAL_MACHINE,
            TEXT("Software\\Classes\\CLSID\\{52A10783-C811-4C45-9A3D-221A962C8640}\\InprocServer32"),
            NULL,
            filename) != ERROR_SUCCESS) {
        return SELFREG_E_CLASS;
    }

    if (setValue(
            HKEY_LOCAL_MACHINE,
            TEXT("Software\\Classes\\CLSID\\{52A10783-C811-4C45-9A3D-221A962C8640}\\InprocServer32"),
            TEXT("ThreadingModel"),
            TEXT("Apartment")) != ERROR_SUCCESS) {
        return SELFREG_E_CLASS;
    }

    if (setValue(
            HKEY_LOCAL_MACHINE,
            TEXT("Software\\Classes\\*\\ShellEx\\ContextMenuHandlers\\NitroShellExt"),
            NULL,
            TEXT("{52A10783-C811-4C45-9A3D-221A962C8640}")) != ERROR_SUCCESS) {
        return SELFREG_E_CLASS;
    }

    if (setValue(
            HKEY_LOCAL_MACHINE,
            TEXT("Software\\Classes\\Folder\\ShellEx\\ContextMenuHandlers\\NitroShellExt"),
            NULL,
            TEXT("{52A10783-C811-4C45-9A3D-221A962C8640}")) != ERROR_SUCCESS) {
        return SELFREG_E_CLASS;
    }

    if (setValue(
            HKEY_LOCAL_MACHINE,
            TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved"),
            TEXT("{52A10783-C811-4C45-9A3D-221A962C8640}"),
            TEXT("NitroShare Context Menu")) != ERROR_SUCCESS) {
        return SELFREG_E_CLASS;
    }

    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, 0, 0);

    return S_OK;
}

STDAPI DllUnregisterServer()
{
    if (deleteKey(HKEY_LOCAL_MACHINE, TEXT("Software\\Classes\\CLSID\\{52A10783-C811-4C45-9A3D-221A962C8640}")) != ERROR_SUCCESS) {
        return SELFREG_E_CLASS;
    }

    if (deleteKey(HKEY_LOCAL_MACHINE, TEXT("Software\\Classes\\*\\ShellEx\\ContextMenuHandlers\\NitroShellExt")) != ERROR_SUCCESS) {
        return SELFREG_E_CLASS;
    }

    if (deleteKey(HKEY_LOCAL_MACHINE, TEXT("Software\\Classes\\Folder\\ShellEx\\ContextMenuHandlers\\NitroShellExt")) != ERROR_SUCCESS) {
        return SELFREG_E_CLASS;
    }

    if (deleteValue(HKEY_LOCAL_MACHINE,
                    TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved"),
                    TEXT("{52A10783-C811-4C45-9A3D-221A962C8640}")) != ERROR_SUCCESS) {
        return SELFREG_E_CLASS;
    }

    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, 0, 0);

    return S_OK;
}
