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

#ifndef LIBNITROSHARE_TRANSPORTSERVER_H
#define LIBNITROSHARE_TRANSPORTSERVER_H

#include <QObject>

#include <nitroshare/config.h>

class Device;
class Transport;

/**
 * @brief Server for receiving and initializing transfers
 */
class NITROSHARE_EXPORT TransportServer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name)

public:

    /**
     * @brief Retrieve the unique identifier for the transport
     */
    virtual QString name() const = 0;

    /**
     * @brief Create a transport for sending items to a device
     * @param device pointer to Device
     * @return pointer to Transport or nullptr
     *
     * This method is invoked to create a transport for sending a bundle to the
     * specified device. The transport server should use the properties in the
     * device to create the transport or return nullptr if it cannot.
     */
    virtual Transport *createTransport(Device *device) = 0;

Q_SIGNALS:

    /**
     * @brief Indicate that an incoming transfer has been received
     * @param transport pointer to Transport
     *
     * The transport should be allocated on the heap and will be properly
     * deleted when no longer needed.
     */
    void transportReceived(Transport *transport);
};

#endif // LIBNITROSHARE_TRANSPORTSERVER_H
