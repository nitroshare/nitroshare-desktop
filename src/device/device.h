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

#ifndef NS_DEVICE_H
#define NS_DEVICE_H

#include <QHostAddress>
#include <QObject>
#include <QSharedPointer>

class DevicePrivate;

/**
 * @brief Device discovered through broadcast
 *
 * A device is considered "expired" if a packet has not been received from the
 * device after a configurable amount of time has elapsed.
 */
class Device : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString uuid READ uuid CONSTANT)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString operatingSystem READ operatingSystem WRITE setOperatingSystem)
    Q_PROPERTY(QHostAddress address READ address)
    Q_PROPERTY(quint16 port READ port)
    Q_PROPERTY(bool expired READ expired)

public:

    /**
     * @brief Create a new device
     * @param uuid the device UUID
     */
    explicit Device(const QString &uuid);

    /**
     * @brief Destroy the device
     */
    virtual ~Device();

    /**
     * @brief Retrieve the UUID of the device
     * @return device UUID
     */
    QString uuid() const;

    /**
     * @brief Retrieve the name of the device
     * @return device name
     */
    QString name() const;

    /**
     * @brief Set the name of the device
     * @param device name
     */
    void setName(const QString &name);

    /**
     * @brief Retrieve the operating system of the device
     * @return device operating system
     */
    QString operatingSystem() const;

    /**
     * @brief Set the operating system of the device
     * @param device operating system
     */
    void setOperatingSystem(const QString &operatingSystem);

    /**
     * @brief Retrieve the address for connecting to the device
     * @return device address
     */
    QHostAddress address() const;

    /**
     * @brief Retrieve the port for connecting to the device
     * @return device port
     */
    quint16 port() const;

    /**
     * @brief Retrieve the expiration status of the device
     * @return true if the device has expired
     */
    bool expired() const;

    /**
     * @brief Update the device address and port
     * @param address device address
     * @param port device port
     *
     * This method will update the timestamp used to determine if the device
     * has expired.
     */
    void update(const QHostAddress &address, quint16 port);

Q_SIGNALS:

    /**
     * @brief Indicate that the name of the device has changed
     * @param name device name
     */
    void nameChanged(const QString &name);

private:

    DevicePrivate * const d;
};

#endif // NS_DEVICE_H
