/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Nathan Osman
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

#ifndef LIBNITROSHARE_TRANSPORTSERVERREGISTRY_H
#define LIBNITROSHARE_TRANSPORTSERVERREGISTRY_H

#include <QObject>

#include <nitroshare/config.h>

class Device;
class Transport;
class TransportServer;

class NITROSHARE_EXPORT TransportServerRegistryPrivate;

/**
 * @brief Registry for transport servers
 */
class NITROSHARE_EXPORT TransportServerRegistry : public QObject
{
    Q_OBJECT

public:

    /**
     * @brief Create a new TransportServer registry
     * @param parent QObject
     */
    explicit TransportServerRegistry(QObject *parent = nullptr);

    /**
     * @brief Add a transport server
     * @param server pointer to TransportServer
     */
    void add(TransportServer *server);

    /**
     * @brief Remove a transport server
     * @param server pointer to TransportServer
     */
    void remove(TransportServer *server);

    /**
     * @brief Create a transport for the specified device
     * @param device pointer to Device
     * @return pointer to Transport or nullptr
     */
    Transport *createTransport(Device *device);

Q_SIGNALS:

    /**
     * @brief Indicate that a new transport was created
     * @param transport pointer to Transport
     */
    void transportReceived(Transport *transport);

private:

    TransportServerRegistryPrivate *const d;
};

#endif // LIBNITROSHARE_TRANSPORTSERVERREGISTRY_H
