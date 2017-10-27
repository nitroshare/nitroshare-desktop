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

#include <nitroshare/application.h>
#include <nitroshare/logger.h>
#include <nitroshare/message.h>
#include <nitroshare/settingsregistry.h>

#include "lantransport.h"
#include "lantransportserver.h"

const QString MessageTag = "lantransportserver";

const QString TransferPort = "TransferPort";

LanTransportServer::LanTransportServer(Application *application)
    : mApplication(application),
      mTransferPort({
          { Setting::TypeKey, Setting::Integer },
          { Setting::NameKey, TransferPort },
          { Setting::TitleKey, tr("Transfer Port") },
          { Setting::DefaultValueKey, 40818 }
      })
{
    connect(&mServer, &Server::newSocketDescriptor, this, &LanTransportServer::onNewSocketDescriptor);
    connect(mApplication->settingsRegistry(), &SettingsRegistry::settingsChanged, this, &LanTransportServer::onSettingsChanged);

    mApplication->settingsRegistry()->add(&mTransferPort);

    // Trigger loading the initial settings
    onSettingsChanged({ TransferPort });
}

LanTransportServer::~LanTransportServer()
{
    mApplication->settingsRegistry()->remove(&mTransferPort);
}

Transport *LanTransportServer::createTransport(const QVariantMap &properties)
{
    return nullptr;
}

void LanTransportServer::onNewSocketDescriptor(qintptr socketDescriptor)
{
    mApplication->logger()->log(new Message(
        Message::Debug,
        MessageTag,
        "socket descriptor for incoming connection received"
    ));

    emit transportReceived(new LanTransport(socketDescriptor));
}

void LanTransportServer::onSettingsChanged(const QStringList &keys)
{
    if (keys.contains(TransferPort)) {
        mServer.close();
        if (!mServer.listen(QHostAddress::Any,
                mApplication->settingsRegistry()->value(TransferPort).toInt())) {
            mApplication->logger()->log(new Message(
                Message::Error,
                MessageTag,
                mServer.errorString()
            ));
        }
    }
}
