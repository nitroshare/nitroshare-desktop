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

    if (!IsEqualIID(riid, IID_IUnknown) &&
            !IsEqualIID(riid, IID_IShellExtInit) &&
            !IsEqualIID(riid, IID_IContextMenu)) {
        return E_NOINTERFACE;
    }

    *ppvObject = this;
    AddRef();
    return NOERROR;
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
    MessageBox(NULL, TEXT("ContextMenu::Initialize"), TEXT("Note:"), 0);

    return E_FAIL;
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
    //...

    return E_FAIL;
}
