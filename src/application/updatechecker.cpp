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

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonDocument>
#include <QJsonValue>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QVariantMap>

#include "../util/json.h"
#include "../util/platform.h"
#include "config.h"
#include "updatechecker.h"

// URL used for querying the API for new versions
const QUrl UpdateUrl = QUrl("http://nitroshare.net/api/1.0/updates/");

// Maximum number of redirects that may be processed
const int MaxRedirects = 2;

UpdateChecker::UpdateChecker()
{
    connect(&mTimer, &QTimer::timeout, this, &UpdateChecker::checkForUpdates);
    connect(&mManager, &QNetworkAccessManager::finished, this, &UpdateChecker::onFinished);
    connect(Settings::instance(), &Settings::settingChanged, this, &UpdateChecker::onSettingChanged);

    // Use a single-shot timer to ensure that it is only
    // started after an update check has completed
    mTimer.setSingleShot(true);
    reload();

    // Check for an update right away
    checkForUpdates();
}

void UpdateChecker::checkForUpdates()
{
    // Reset the number of redirects and begin the request
    mRedirectsRemaining = MaxRedirects;
    sendRequest(UpdateUrl);
}

void UpdateChecker::onFinished(QNetworkReply *reply)
{
    // Before doing anything else, indicate that we're done with the reply
    reply->deleteLater();

    // Now, one of three things must have happened:
    // - there was an error
    // - the request succeeded and we parse the JSON
    // - the server is redirecting us to a new URL
    if(reply->error() == QNetworkReply::NoError) {

        // Check if a redirect URL was supplied
        QUrl newUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
        if(!newUrl.isValid()) {

            // Begin extracting data from the JSON returned
            QJsonDocument document = QJsonDocument::fromJson(reply->readAll());
            QJsonArray array;
            QJsonObject object;
            QString version;
            QString url;

            // Verify that an array was received
            if(Json::isArray(document, array)) {

                // Check to see if a new version exists
                if(array.count()) {

                    // A new version exists, extract the version and URL
                    if(Json::isObject(array.at(0), object) &&
                            Json::objectContains(object, "version", version) &&
                            Json::objectContains(object, "url", url)) {

                        // Indicate that a new version was released
                        emit newVersion(version, QUrl(url));
                        return;
                    }

                } else {

                    // No new version, just return
                    return;
                }
            }

            qWarning("Malformed JSON received from update server.");

        } else if(mRedirectsRemaining) {

            // Use the original URL to resolve the new location, decrement
            // the number of redirects remaining, and immediately return
            sendRequest(reply->url().resolved(newUrl));
            --mRedirectsRemaining;
            return;

        } else {
            qWarning("Maximum number of redirects exceeded while checking for updates.");
        }
    } else {
        qWarning("Unable to check for updates.");
    }

    // Schedule the next check
    mTimer.start();
}

void UpdateChecker::onSettingChanged(Settings::Key key)
{
    if(key == Settings::UpdateInterval) {
        reload();
    }
}

void UpdateChecker::sendRequest(const QUrl &url)
{
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // In order for the API to provide the correct update URL,
    // we need to provide the current version, OS, and CPU architecture
    QVariantMap data = {
        { "version", PROJECT_VERSION },
        { "operating_system", Platform::operatingSystemName() },
        { "architecture", Platform::architectureName() }
    };

    // Include the information in the request
    QJsonObject object = QJsonObject::fromVariantMap(data);
    mManager.post(request, QJsonDocument(object).toJson());
}

void UpdateChecker::reload()
{
    mTimer.setInterval(Settings::get<int>(Settings::UpdateInterval));
}
