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

#include "config.h"

#include <QHostAddress>

#ifdef ENABLE_TLS
#  include <QFile>
#endif

#include <nitroshare/application.h>
#include <nitroshare/category.h>
#include <nitroshare/device.h>
#include <nitroshare/logger.h>
#include <nitroshare/message.h>
#include <nitroshare/settingsregistry.h>

#include "lantransport.h"
#include "lantransportserver.h"

const QString MessageTag = "lantransportserver";

const QString TransferCategory = "transfer";
const QString TransferPort = "TransferPort";
#ifdef ENABLE_TLS
const QString TlsEnabled = "TlsEnabled";
const QString TlsCaCertificate = "TlsCaCertificate";
const QString TlsCertificate = "TlsCertificate";
const QString TlsPrivateKey = "TlsPrivateKey";
const QString TlsPrivateKeyPassphrase = "TlsPrivateKeyPassphrase";
#endif

LanTransportServer::LanTransportServer(Application *application)
    : mApplication(application)
    , mTransferCategory({
          { Category::NameKey, TransferCategory },
          { Category::TitleKey, tr("Transfer") }
      })
    , mTransferPort({
          { Setting::TypeKey, Setting::Integer },
          { Setting::NameKey, TransferPort },
          { Setting::TitleKey, tr("Transfer Port") },
          { Setting::CategoryKey, TransferCategory },
          { Setting::DefaultValueKey, 40818 }
      })
#ifdef ENABLE_TLS
    , mTlsEnabled({
          { Setting::TypeKey, Setting::Boolean },
          { Setting::NameKey, TlsEnabled },
          { Setting::TitleKey, tr("Enable TLS") },
          { Setting::CategoryKey, TransferCategory }
      })
    , mTlsCaCertificate({
          { Setting::TypeKey, Setting::FilePath },
          { Setting::NameKey, TlsCaCertificate },
          { Setting::TitleKey, tr("CA certificate") },
          { Setting::CategoryKey, TransferCategory }
      })
    , mTlsCertificate({
          { Setting::TypeKey, Setting::FilePath },
          { Setting::NameKey, TlsCertificate },
          { Setting::TitleKey, tr("Certificate") },
          { Setting::CategoryKey, TransferCategory }
      })
    , mTlsPrivateKey({
          { Setting::TypeKey, Setting::FilePath },
          { Setting::NameKey, TlsPrivateKey },
          { Setting::TitleKey, tr("Private key") },
          { Setting::CategoryKey, TransferCategory }
      })
    , mTlsPrivateKeyPassphrase({
          { Setting::TypeKey, Setting::String },
          { Setting::NameKey, TlsPrivateKeyPassphrase },
          { Setting::TitleKey, tr("Private key passphrase") },
          { Setting::CategoryKey, TransferCategory }
      })
#endif
{
    connect(&mServer, &Server::newSocketDescriptor, this, &LanTransportServer::onNewSocketDescriptor);
    connect(mApplication->settingsRegistry(), &SettingsRegistry::settingsChanged, this, &LanTransportServer::onSettingsChanged);

    mApplication->settingsRegistry()->addCategory(&mTransferCategory);
    mApplication->settingsRegistry()->addSetting(&mTransferPort);
#ifdef ENABLE_TLS
    mApplication->settingsRegistry()->addSetting(&mTlsEnabled);
    mApplication->settingsRegistry()->addSetting(&mTlsCaCertificate);
    mApplication->settingsRegistry()->addSetting(&mTlsCertificate);
    mApplication->settingsRegistry()->addSetting(&mTlsPrivateKey);
    mApplication->settingsRegistry()->addSetting(&mTlsPrivateKeyPassphrase);
#endif

    // Trigger loading the initial settings
    onSettingsChanged({
        TransferPort
#ifdef ENABLE_TLS
      , TlsEnabled
#endif
    });
}

LanTransportServer::~LanTransportServer()
{
    mApplication->settingsRegistry()->removeSetting(&mTransferPort);
#ifdef ENABLE_TLS
    mApplication->settingsRegistry()->removeSetting(&mTlsEnabled);
    mApplication->settingsRegistry()->removeSetting(&mTlsCaCertificate);
    mApplication->settingsRegistry()->removeSetting(&mTlsCertificate);
    mApplication->settingsRegistry()->removeSetting(&mTlsPrivateKey);
    mApplication->settingsRegistry()->removeSetting(&mTlsPrivateKeyPassphrase);
#endif
    mApplication->settingsRegistry()->removeCategory(&mTransferCategory);
}

QString LanTransportServer::name() const
{
    return "lan";
}

Transport *LanTransportServer::createTransport(Device *device)
{
    QStringList addresses = device->property("addresses").toStringList();
    quint16 port = device->property("port").toInt();

    // Verify that valid data was passed
    if (!addresses.count() || !port) {
        mApplication->logger()->log(new Message(
            Message::Error,
            MessageTag,
            QString("invalid addresses or port: %1, %2")
                .arg(addresses.join(", "))
                .arg(port)
        ));
        return nullptr;
    }

    // Create the transport
    return new LanTransport(
        QHostAddress(addresses.at(0))
      , port
#ifdef ENABLE_TLS
      , mSslConf
#endif
    );
}

void LanTransportServer::onNewSocketDescriptor(qintptr socketDescriptor)
{
    mApplication->logger()->log(new Message(
        Message::Debug,
        MessageTag,
        "socket descriptor for incoming connection received"
    ));

    emit transportReceived(new LanTransport(
        socketDescriptor
#ifdef ENABLE_TLS
      , mSslConf
#endif
    ));
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

#ifdef ENABLE_TLS
    if (keys.contains(TlsEnabled) ||
            keys.contains(TlsCaCertificate) ||
            keys.contains(TlsCertificate) ||
            keys.contains(TlsPrivateKey) ||
            keys.contains(TlsPrivateKeyPassphrase)) {
        if (mApplication->settingsRegistry()->value(TlsEnabled).toBool()) {

            // Ensure the peer is verified as well
            mSslConf.setPeerVerifyMode(QSslSocket::VerifyPeer);

            // Set the root (CA) certificate
            mSslConf.setCaCertificates({
                loadCert(mApplication->settingsRegistry()->value(TlsCaCertificate).toString())
            });

            // Specify the certificate to use for this device
            mSslConf.setLocalCertificate(
                loadCert(mApplication->settingsRegistry()->value(TlsCertificate).toString())
            );

            // Set the private key and passphrase
            mSslConf.setPrivateKey(loadKey(
                mApplication->settingsRegistry()->value(TlsPrivateKey).toString(),
                mApplication->settingsRegistry()->value(TlsPrivateKeyPassphrase).toString()
            ));

        } else {
            mSslConf = QSslConfiguration();
        }
    }
#endif
}

#ifdef ENABLE_TLS

QSslCertificate LanTransportServer::loadCert(const QString &filename) const
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        return QSslCertificate();
    }
    QSslCertificate cert(&file);
    file.close();
    return cert;
}

QSslKey LanTransportServer::loadKey(const QString &filename, const QString &passphrase) const
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        return QSslKey();
    }
    QSslKey key(&file, QSsl::Rsa, QSsl::Pem, QSsl::PrivateKey, passphrase.toUtf8());
    file.close();
    return key;
}

#endif
