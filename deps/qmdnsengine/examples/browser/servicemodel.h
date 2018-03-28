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
 */

#ifndef SERVICEMODEL_H
#define SERVICEMODEL_H

#include <QAbstractListModel>
#include <QList>

#include <qmdnsengine/browser.h>
#include <qmdnsengine/cache.h>
#include <qmdnsengine/service.h>
#include <qmdnsengine/server.h>

class ServiceModel : public QAbstractListModel
{
    Q_OBJECT

public:

    ServiceModel(QMdnsEngine::Server *server, const QByteArray &type);

    virtual int rowCount(const QModelIndex &parent) const;
    virtual QVariant data(const QModelIndex &index, int role) const;

private Q_SLOTS:

    void onServiceAdded(const QMdnsEngine::Service &service);
    void onServiceUpdated(const QMdnsEngine::Service &service);
    void onServiceRemoved(const QMdnsEngine::Service &service);

private:

    int findService(const QByteArray &name);

    QMdnsEngine::Cache cache;
    QMdnsEngine::Browser mBrowser;
    QList<QMdnsEngine::Service> mServices;
};

#endif // SERVICEMODEL_H
