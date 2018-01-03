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
 */

#include <nitroshare/action.h>
#include <nitroshare/actionregistry.h>
#include <nitroshare/application.h>
#include <nitroshare/logger.h>
#include <nitroshare/message.h>

#include "url.h"
#include "urlhandler.h"

const QString MessageTag = "url";

UrlHandler::UrlHandler(Application *application)
    : mApplication(application)
{
}

QString UrlHandler::name() const
{
    return "url";
}

Item *UrlHandler::createItem(const QString &, const QVariantMap &properties)
{
    Url *url = new Url(properties);
    connect(url, &Url::openUrl, this, &UrlHandler::onOpenUrl);
    return url;
}

void UrlHandler::onOpenUrl(const QString &url)
{
    // Attempt to find the openurl action
    Action *action = mApplication->actionRegistry()->find("openurl");
    if (!action) {
        mApplication->logger()->log(new Message(
            Message::Error,
            MessageTag,
            "unable to find the \"openurl\" action"
        ));
        return;
    }

    // Invoke the action with the URL
    if (!action->invoke({ { "url", url } }).toBool()) {
        mApplication->logger()->log(new Message(
            Message::Error,
            MessageTag,
            QString("unable to open the URL \"%1\"").arg(url)
        ));
    }
}
