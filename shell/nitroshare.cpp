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

#include <cctype>
#include <sstream>

#include <Shlobj.h>
#include <strsafe.h>
#include <windows.h>

#include "nitroshare.h"

bool getProfileDir(std::wstring &path)
{
    PWSTR pszPath;
    if (SHGetKnownFolderPath(FOLDERID_Profile, 0, NULL, &pszPath) != S_OK) {
        return false;
    }
    path.assign(pszPath);
    CoTaskMemFree(pszPath);
    return true;
}

bool readConfigFile(const std::wstring &file, std::string &content)
{
    HANDLE hFile = CreateFileW(file.data(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        return false;
    }
    bool bRet = true;
    while (true) {
        CHAR data[MAX_PATH];
        DWORD nBytes;
        if (ReadFile(hFile, data, MAX_PATH, &nBytes, NULL) == FALSE) {
            bRet = false;
            break;
        }
        if (nBytes == 0) {
            break;
        }
        content.append(data, nBytes);
    }
    CloseHandle(hFile);
    return bRet;
}

bool parseConfigFile(const std::string &content, int &port, std::string &token)
{
    // Using a regex for this would not only be difficult but error prone
    // So instead we create a really, really basic JSON parser
    enum {
        SF_BRACE,  SF_PROP,       SF_PROP_NAME,
        SF_COLON,  SF_VAL,        SF_NUMBER,
        SF_STRING, SF_STRING_ESC, SF_COMMA,
        SF_DONE
    } state = SF_BRACE;

    std::string a;    // accumulator
    std::string prop; // current property name

    const char *p = content.data();
    const char *end = content.data() + content.length();

    for (; p != end; ++p) {
        switch (state) {
        case SF_BRACE: case SF_PROP:
        case SF_COLON: case SF_VAL:
        case SF_COMMA: case SF_DONE:
            if (isspace(*p)) {
                continue;
            }
        }
        switch (state) {
        case SF_BRACE:
            if (*p != '{') {
                return false;
            }
            state = SF_PROP;
            break;
        case SF_PROP:
            if (*p != '"') {
                return false;
            }
            state = SF_PROP_NAME;
            a.clear();
            break;
        case SF_PROP_NAME:
            if (*p == '"') {
                prop = a;
                state = SF_COLON;
                continue;
            }
            a += *p;
            break;
        case SF_COLON:
            if (*p != ':') {
                return false;
            }
            state = SF_VAL;
            a.clear();
            break;
        case SF_VAL:
            if (*p >= '0' && *p <= '9') {
                state = SF_NUMBER;
                a += *p;
            } else if (*p == '"') {
                state = SF_STRING;
            }
            break;
        case SF_NUMBER:
            if (*p < '0' || *p > '9') {
                if (prop == "port") {
                    port = atoi(a.data());
                }
                state = SF_COMMA;
                --p;
                continue;
            }
            a += *p;
            break;
        case SF_STRING:
            if (*p == '"') {
                if (prop == "token") {
                    token = a;
                }
                state = SF_COMMA;
                continue;
            } else if (*p == '\\') {
                state = SF_STRING_ESC;
                continue;
            }
            a += *p;
            break;
        case SF_STRING_ESC:
            a += *p;
            state = SF_STRING;
            break;
        case SF_COMMA:
            if (*p == ',') {
                state = SF_PROP;
                continue;
            } else if (*p == '}') {
                state = SF_DONE;
            } else {
                return false;
            }
            break;
        case SF_DONE:
            break;
        }
    }

    return true;
}

bool findNitroShare(int &port, std::string &token)
{
    // Calculate the path to the config file
    std::wstring path;
    if (!getProfileDir(path)) {
        return false;
    }
    path.append(L"\\.NitroShare");

    // Read the config file's contents
    std::string content;
    if (!readConfigFile(path, content)) {
        return false;
    }

    return parseConfigFile(content, port, token);
}

std::wstring escape(const std::wstring &src)
{
    std::wstring d;
    for (std::wstring::const_iterator i = src.begin(); i != src.end(); ++i) {
        switch (*i) {
        case '"':
            d += '\\';
            break;
        }
        d += *i;
    }
    return d;
}

bool sendItems(int port, const std::string &token, const std::vector<std::wstring> &items)
{
    TCHAR url[MAX_PATH];
    StringCbPrintf(url, MAX_PATH, TEXT("http://localhost:%d/sendItems"), port);

    // Combine the filenames
    std::wstringstream stream;
    stream << "{\"items\":[";
    for (std::vector<std::wstring>::const_iterator i = items.begin(); i != items.end(); ++i) {
        stream << (i != items.begin() ? ", " : "")
               << "\""
               << escape(*i)
               << "\"";
    }
    stream << "]}";

    // Convert the UTF-16 string into a UTF-8 one
    std::wstring data = stream.str();
    int len = WideCharToMultiByte(CP_UTF8, 0, data.c_str(), -1, NULL, 0, NULL, NULL);
    char *json = new char[len];
    WideCharToMultiByte(CP_UTF8, 0, data.c_str(), -1, json, len, NULL, NULL);

    // TODO: send the JSON with the token

    // Free the UTF-8 data
    delete[] json;

    return true;
}
