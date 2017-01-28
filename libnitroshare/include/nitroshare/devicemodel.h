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

#ifndef LIBNITROSHARE_DEVICEMODEL_H
#define LIBNITROSHARE_DEVICEMODEL_H

#include <QAbstractListModel>

#include <nitroshare/config.h>

class DeviceEnumerator;

class NITROSHARE_EXPORT DeviceModelPrivate;

/**
 * @brief Model representing a list of devices
 */
class NITROSHARE_EXPORT DeviceModel : public QAbstractListModel
{
    Q_OBJECT

public:

    enum Role {
        UuidRole = Qt::UserRole,
        NameRole,
        VersionRole,
        OperatingSystemRole,
        AddressesRole,
        PortRole,
        TlsRole
    };

    /**
     * @brief Create a device model
     * @param parent QObject
     */
    explicit DeviceModel(QObject *parent = nullptr);

    /**
     * @brief Add an enumerator to the model
     * @param enumerator device enumerator
     */
    void addEnumerator(DeviceEnumerator *enumerator);

    /**
     * @brief Remove the enumerator from the model
     * @param enumerator device enumerator
     */
    void removeEnumerator(DeviceEnumerator *enumerator);

    // Reimplemented virtual methods
    virtual int rowCount(const QModelIndex &parent) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual QHash<int, QByteArray> roleNames() const;

private:

    DeviceModelPrivate *const d;
    friend class DeviceModelPrivate;
};

#endif // LIBNITROSHARE_DEVICEMODEL_H
