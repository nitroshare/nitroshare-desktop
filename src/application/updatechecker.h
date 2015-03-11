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

#ifndef NS_UPDATECHECKER_H
#define NS_UPDATECHECKER_H

#include <QNetworkAccessManager>
#include <QTimer>
#include <QUrl>

#include "../settings/settings.h"

/**
 * @brief Periodically checks for application updates
 *
 * In order to ensure that users stay secure and receive bug fixes, it is
 * important that they receive notifications when updates are available. This
 * class periodically checks for new releases and notifies the user when one
 * is available. This is done by making an HTTP request to a predefined URL.
 *
 * To use this class, simply create an instance that will persist while the
 * application is running. It will check for updates at a preset interval.
 */
class UpdateChecker : public QObject
{
    Q_OBJECT

public:

    UpdateChecker();

Q_SIGNALS:

    void newVersion(const QString &version, const QUrl &url);

private Q_SLOTS:

    void checkForUpdates();
    void onFinished(QNetworkReply *reply);

    void onSettingsChanged(const QList<Settings::Key> &keys = {});

private:

    void sendRequest(const QUrl &url);

    QTimer mTimer;
    QNetworkAccessManager mManager;

    int mRedirectsRemaining;
};

#endif // NS_UPDATECHECKER_H
