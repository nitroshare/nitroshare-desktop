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

#include <QFileInfo>

#include <nitroshare/application.h>
#include <nitroshare/bundle.h>
#include <nitroshare/device.h>
#include <nitroshare/devicemodel.h>
#include <nitroshare/transfer.h>
#include <nitroshare/transfermodel.h>
#include <nitroshare/transport.h>
#include <nitroshare/transportserver.h>

#include "file.h"
#include "sendaction.h"

SendAction::SendAction(Application *application)
    : mApplication(application)
{
}

QString SendAction::name() const
{
    return "send";
}

QVariant SendAction::invoke(const QVariantMap &params)
{
    // Attempt to find the device
    Device *device = mApplication->deviceModel()->findDevice(
        params.value("device").toString(),
        params.value("enumerator").toString()
    );
    if (!device) {
        return false;
    }

    // Find the transport server
    TransportServer *server = mApplication->transferModel()->findTransportServer(
        device->transportName()
    );
    if (!server) {
        return false;
    }

    // Create a transport
    Transport *transport = server->createTransport(device);
    if (!transport) {
        return false;
    }

    //////////
    // TODO //
    //////////

    // Create a bundle from the list of items to send
    Bundle *bundle = new Bundle;

    foreach (const QVariant &item, params.value("items").toList()) {
        QFileInfo info(item.toString());
        bundle->add(new File(info.path(), info, 65536));
    }

    // Create a new transfer and add it to the transfer model
    Transfer *transfer = new Transfer(mApplication, transport, bundle);
    mApplication->transferModel()->addTransfer(transfer);
    return true;
}
