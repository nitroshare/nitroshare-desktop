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

#ifndef NS_TRANSFER_H
#define NS_TRANSFER_H

#include <QObject>
#include <QSharedPointer>

#include "../device/device.h"
#include "../filesystem/bundle.h"

class TransferPrivate;

/**
 * @brief Transfer currently in progress
 *
 * Transfers take place in a secondary thread, allowing blocking read and write
 * operations to run without interfering with the UI. The Transfer class is
 * used both for sending and receiving files.
 */
class Transfer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString deviceName READ deviceName NOTIFY deviceNameChanged)
    Q_PROPERTY(int progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(QString error READ error)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(Direction direction READ direction CONSTANT)
    Q_ENUMS(Status)
    Q_ENUMS(Direction)

public:

    /**
     * @brief Status of the transfer
     */
    enum Status {
        /// Transfer has not yet begun
        Pending,
        /// Transfer is currently in progress
        InProgress,
        /// Transfer encountered an error
        Error,
        /// Transfer completed without error
        Completed
    };

    /**
     * @brief Direction of the transfer
     */
    enum Direction {
        /// Files are being sent
        Send,
        /// Files are being received
        Receive
    };

    /**
     * @brief Create a new transfer for receiving files
     * @param socketDescriptor open descriptor for the socket
     */
    Transfer(qintptr socketDescriptor);

    /**
     * @brief Create a new transfer for sending a bundle
     * @param device device to send the bundle to
     * @param bundle bundle to send to the specified device
     */
    Transfer(const Device *device, BundlePointer bundle);

    /**
     * @brief Retrieve the name of the connected device
     * @return name of the device
     */
    QString deviceName() const;

    /**
     * @brief Retrieve the progress of the transfer
     * @return transfer progress between 0 and 100
     */
    int progress() const;

    /**
     * @brief Retrieve the error message (if any)
     * @return error message or empty string if none
     */
    QString error() const;

    /**
     * @brief Retrieve the status of the transfer
     * @return transfer status
     */
    Status status() const;

    /**
     * @brief Retrieve the direction of the transfer (sending or receiving)
     * @return transfer direction
     */
    Direction direction() const;

Q_SIGNALS:

    /**
     * @brief Indicate that the connected device name has changed
     * @param deviceName name of the device
     */
    void deviceNameChanged(const QString &deviceName);

    /**
     * @brief Indicate that the progress of the transfer has changed
     * @param progress transfer progress between 0 and 100
     */
    void progressChanged(int progress);

    /**
     * @brief Indicate that the status of the transfer has changed
     * @param status status of the transfer
     */
    void statusChanged(Status status);

public Q_SLOTS:

    /**
     * @brief Begin the transfer
     */
    void start();

    /**
     * @brief Cancel the transfer
     */
    void cancel();

private:

    TransferPrivate * const d;
};

/**
 * @brief Shared pointer to a Transfer instance
 */
typedef QSharedPointer<Transfer> TransferPointer;

Q_DECLARE_METATYPE(TransferPointer)

#endif // NS_TRANSFER_H
