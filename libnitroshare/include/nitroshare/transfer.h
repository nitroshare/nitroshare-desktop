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

#ifndef LIBNITROSHARE_TRANSFER_H
#define LIBNITROSHARE_TRANSFER_H

#include <QObject>

#include <nitroshare/config.h>

class Bundle;
class HandlerRegistry;
class Settings;
class Transport;

class NITROSHARE_EXPORT TransferPrivate;

/**
 * @brief Transfer between two devices
 *
 * This class implements the protocol for performing a transfer between peers.
 * The two constructors determine whether the transfer sends or receives items
 * since a transfer sending items requires a bundle to transfer.
 */
class NITROSHARE_EXPORT Transfer : public QObject
{
    Q_OBJECT
    Q_ENUMS(Direction)
    Q_ENUMS(State)
    Q_PROPERTY(Direction direction READ direction)
    Q_PROPERTY(State state READ state NOTIFY stateChanged)
    Q_PROPERTY(int progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(QString deviceName READ deviceName NOTIFY deviceNameChanged)
    Q_PROPERTY(QString error READ error NOTIFY errorChanged)

public:

    /**
     * @brief Direction of transfer
     */
    enum Direction {
        Send,
        Receive
    };

    /**
     * @brief State of transfer
     */
    enum State {
        /// Transport is in the process of connecting to peer
        Connecting,
        /// Transfer is in progress
        InProgress,
        /// An error occurred during transfer
        Failed,
        /// Transfer has completed successfully
        Succeeded
    };

    /**
     * @brief Create a transfer to receive items
     * @param handlerRegistry pointer to HandlerRegistry
     * @param transport pointer to Transport
     * @param parent QObject
     */
    Transfer(HandlerRegistry *handlerRegistry, Transport *transport, QObject *parent = nullptr);

    /**
     * @brief Create a transfer to send items
     * @param handlerRegistry pointer to HandlerRegistry
     * @param transport pointer to Transport
     * @param bundle pointer to Bundle
     * @param parent QObject
     */
    Transfer(HandlerRegistry *handlerRegistry, Transport *transport, Bundle *bundle, QObject *parent = nullptr);

    /**
     * @brief Retrieve the direction of transfer
     * @return transfer direction
     */
    Direction direction() const;

    /**
     * @brief Retrieve the current state of transfer
     * @return transfer state
     */
    State state() const;

    /**
     * @brief Retrieve the progress of the transfer
     * @return integer between 0 and 100 inclusive
     *
     * This value is only computed (at most) every half-second to avoid
     * frequent (and unnecessary) UI updates.
     */
    int progress() const;

    /**
     * @brief Retrieve the name of the remote peer
     * @return device name
     */
    QString deviceName() const;

    /**
     * @brief Retrieve a description of an error
     * @return error message
     */
    QString error() const;

Q_SIGNALS:

    /**
     * @brief Indicate that the state of the transfer has changed
     * @param state new state
     */
    void stateChanged(State state);

    /**
     * @brief Indicate that transfer progress has changed
     * @param progress new progress
     */
    void progressChanged(int progress);

    /**
     * @brief Indicate that the name of the remote peer has changed
     * @param deviceName name of the remote peer
     */
    void deviceNameChanged(QString deviceName);

    /**
     * @brief Indicate that a transfer error has occurred
     * @param error description of error
     */
    void errorChanged(QString error);

public Q_SLOTS:

    /**
     * @brief Attempt to gracefully cancel the transfer
     */
    void cancel();

private:

    TransferPrivate *const d;
};

#endif // LIBNITROSHARE_TRANSFER_H
