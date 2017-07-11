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

#include <string>

#include <windows.h>
#include <shellapi.h>

#include "contextmenu.h"
#include "nitroshare.h"

ContextMenu::ContextMenu()
    : mRefCount(1)
{
}

STDMETHODIMP ContextMenu::QueryInterface(REFIID riid, LPVOID *ppvObject)
{
    if (!ppvObject) {
        return E_INVALIDARG;
    }

    *ppvObject = NULL;

    if (IsEqualIID(riid, IID_IUnknown)) {
        OutputDebugString(TEXT("ContextMenu queried for IUnknown"));
        *ppvObject = this;
    } else if (IsEqualIID(riid, IID_IShellExtInit)) {
        OutputDebugString(TEXT("ContextMenu queried for IShellExtInit"));
        *ppvObject = (IShellExtInit*) this;
    } else if (IsEqualIID(riid, IID_IContextMenu)) {
        OutputDebugString(TEXT("ContextMenu queried for IContextMenu"));
        *ppvObject = (IContextMenu*) this;
    } else {
        return E_NOINTERFACE;
    }

    AddRef();
    return S_OK;
}

STDMETHODIMP_(ULONG) ContextMenu::AddRef()
{
    InterlockedIncrement(&mRefCount);
    return mRefCount;
}

STDMETHODIMP_(ULONG) ContextMenu::Release()
{
    ULONG refCount = InterlockedDecrement(&mRefCount);
    if (!mRefCount) {
        delete this;
    }
    return refCount;
}

STDMETHODIMP ContextMenu::Initialize(PCIDLIST_ABSOLUTE pidlFolder, IDataObject *pdtobj, HKEY hkeyProgID)
{
    FORMATETC formatetc = {CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
    STGMEDIUM medium;

    HRESULT hResult = pdtobj->GetData(&formatetc, &medium);
    if (hResult != S_OK) {
        return hResult;
    }

    // Lock the memory for reading the list of files
    HDROP hDrop = (HDROP) GlobalLock(medium.hGlobal);
    if (hDrop == NULL) {
        return E_FAIL;
    }

    // Read the filenames into the list
    mFilenames.clear();
    UINT nFiles = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);
    for (UINT i = 0; i < nFiles; ++i) {
        WCHAR filename[MAX_PATH];
        if (!DragQueryFileW(hDrop, i, filename, MAX_PATH)) {
            hResult = E_FAIL;
        }
        mFilenames.push_back(std::wstring(filename));
    }

    // Free the memory
    GlobalUnlock(medium.hGlobal);

    return hResult;
}

STDMETHODIMP ContextMenu::GetCommandString(UINT_PTR idCmd, UINT uType, UINT *pReserved, LPSTR pszName, UINT cchMax)
{
    //...

    return E_FAIL;
}

STDMETHODIMP ContextMenu::InvokeCommand(LPCMINVOKECOMMANDINFO pici)
{
    //...

    return E_FAIL;
}

STDMETHODIMP ContextMenu::QueryContextMenu(HMENU hmenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags)
{
    if (uFlags & CMF_DEFAULTONLY) {
        return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, 0);
    }

    // Insert the new menu item
    if(!InsertMenu(hmenu, indexMenu, MF_BYPOSITION, idCmdFirst, TEXT("Send with NitroShare"))) {
        return E_FAIL;
    }

    return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, 1);
}
