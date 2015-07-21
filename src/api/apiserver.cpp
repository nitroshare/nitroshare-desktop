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

#include <QJsonDocument>
#include <QJsonObject>
#include <QUuid>

#include "apiserver.h"

ApiServer::ApiServer()
{
    setHandler(&mHandler);
}

ApiServer::~ApiServer()
{
    // The auth file only exists while the app is running
    mAuthFile.remove();
}

bool ApiServer::start()
{
    // Listen for incoming requests on any available port
    if(!listen(QHostAddress::LocalHost)) {
        return false;
    }

    // Open the auth file
    if(!mAuthFile.open()) {
        return false;
    }

    // Generate the JSON contents of the auth file
    QJsonObject object(QJsonObject::fromVariantMap({
        { "token", QUuid::createUuid().toString() },
        { "port", QString::number(serverPort()) }
    }));

    // Write the contents to the auth file and close it
    mAuthFile.write(QJsonDocument(object).toJson());
    mAuthFile.close();

    return true;
}
