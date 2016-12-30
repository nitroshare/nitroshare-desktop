/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Nathan Osman
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

#ifndef LIBNITROSHARE_PLUGINMODEL_H
#define LIBNITROSHARE_PLUGINMODEL_H

#include <QAbstractListModel>

#include <nitroshare/config.h>

class Application;

class NITROSHARE_EXPORT PluginModelPrivate;

/**
 * @brief Model representing currently loaded plugins
 */
class NITROSHARE_EXPORT PluginModel : public QAbstractListModel
{
    Q_OBJECT

public:

    enum Role {
        FilenameRole = Qt::UserRole
    };

    /**
     * @brief Create a new plugin manager
     * @param application pointer to application
     * @param parent QObject
     */
    PluginModel(Application *application, QObject *parent = nullptr);

    /**
     * @brief Add the plugins in the specified directory
     * @param directory absolute path
     */
    void addPlugins(const QString &directory);

    // Reimplemented virtual methods
    virtual int rowCount(const QModelIndex &parent) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual QHash<int, QByteArray> roleNames() const;

private:

    PluginModelPrivate *const d;
};

#endif // LIBNITROSHARE_PLUGINMODEL_H
