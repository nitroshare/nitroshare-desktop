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

#include <QHostAddress>
#include <QTcpSocket>

#include "../filesystem/bundle.h"

class TransferPrivate;

/**
 * @brief Transfer currently in progress
 *
 * A transfer is initiated either by the user attempting to send a file or by
 * an incoming TCP connection. The connection is processed asynchronously.
 */
class Transfer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Direction direction READ direction CONSTANT)
    Q_PROPERTY(State state READ state NOTIFY stateChanged)
    Q_PROPERTY(QString deviceName READ deviceName NOTIFY deviceNameChanged)
    Q_PROPERTY(int progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(QString error READ error)
    Q_ENUMS(Direction)
    Q_ENUMS(Status)

public:

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
     * @brief State of the transfer
     */
    enum State {
        /// Connection is in progress
        Connecting,
        /// Transfer is currently in progress
        InProgress,
        /// Transfer was canceled
        Canceled,
        /// Transfer failed to complete
        Failed,
        /// Transfer completed successfully
        Succeeded
    };

    /**
     * @brief Create a new transfer for receiving files
     * @param socketDescriptor socket descriptor
     * @param parent parent QObject
     */
    Transfer(qintptr socketDescriptor, QObject *parent = nullptr);

    /**
     * @brief Create a new transfer for sending a bundle
     * @param deviceName name of the device
     * @param address address of device
     * @param port port of device
     * @param bundle bundle to send to the specified device
     * @param parent parent QObject
     */
    Transfer(const QString &deviceName, const QHostAddress &address, quint16 port, BundlePointer bundle, QObject *parent = nullptr);

    /**
     * @brief Retrieve the direction of the transfer (sending or receiving)
     * @return transfer direction
     */
    Direction direction() const;

    /**
     * @brief Retrieve the state of the transfer
     * @return transfer state
     */
    State state() const;

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
     * @brief Indicate that the state of the transfer has changed
     * @param state state of the transfer
     */
    void stateChanged(State state);

public Q_SLOTS:

    /**
     * @brief Cancel the transfer
     */
    void cancel();

    /**
     * @brief Start the transfer again
     */
    void restart();

private:

    TransferPrivate * const d;
};

#endif // NS_TRANSFER_H
