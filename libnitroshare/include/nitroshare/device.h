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

#ifndef LIBNITROSHARE_DEVICE_H
#define LIBNITROSHARE_DEVICE_H

#include <QObject>
#include <QStringList>

#include "config.h"

class NITROSHARE_EXPORT DevicePrivate;

/**
 * @brief Peer available for transfers
 */
class NITROSHARE_EXPORT Device : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString uuid READ uuid)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QStringList addresses READ addresses NOTIFY addressesChanged)
    Q_PROPERTY(quint16 port READ port WRITE setPort NOTIFY portChanged)

public:

    /**
     * @brief Create an uninitialized device object
     * @param parent QObject
     */
    Device(const QString &uuid, QObject *parent = nullptr);

    /**
     * @brief Retrieve device identifier
     * @return unique device identifier
     */
    QString uuid() const;

    /**
     * @brief Retrieve device name
     * @return human-friendly device identifier
     */
    QString name() const;

    /**
     * @brief Set device name
     * @param name human-friendly device identifier
     */
    void setName(const QString &name);

    /**
     * @brief Retrieve device addresses
     * @return peer remote addresses
     */
    QStringList addresses() const;

    /**
     * @brief Add a device address
     * @param address peer remote address
     */
    Q_INVOKABLE void addAddress(const QString &address);

    /**
     * @brief Retrieve transfer port
     * @return file transfer port
     */
    quint16 port() const;

    /**
     * @brief Set transfer port
     * @param port file transfer port
     */
    void setPort(quint16 port);

Q_SIGNALS:

    /**
     * @brief Indicate the device name changed
     * @param name human-friendly device identifier
     */
    void nameChanged(const QString &name);

    /**
     * @brief Indicate the list of device addresses has changed
     * @param addresses peer remote addresses
     */
    void addressesChanged(const QStringList &addresses);

    /**
     * @brief Indicate the transfer port changed
     * @param port file transfer port
     */
    void portChanged(quint16 port);

private:

    DevicePrivate *const d;
};

#endif // LIBNITROSHARE_DEVICE_H
