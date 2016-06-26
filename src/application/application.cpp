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

#include <QApplication>
#include <QDateTime>
#include <QDesktopServices>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QStyle>

#include "../device/devicedialog.h"
#include "../icon/trayicon.h"
#include "../settings/settings.h"
#include "../settings/settingsdialog.h"
#include "application.h"
#include "aboutdialog.h"

#ifdef APPINDICATOR_FOUND
#include "../icon/indicatoricon.h"
#include "../util/platform.h"
#endif

// Enable QHostAddress to be used in a QVariant
Q_DECLARE_METATYPE(QHostAddress)

Application::Application()
    : mTransferWindow(&mTransferModel),
#ifdef APPINDICATOR_FOUND
      mIcon(Platform::useIndicator() ? static_cast<Icon*>(new IndicatorIcon) :
                                       static_cast<Icon*>(new TrayIcon)),
#else
      mIcon(new TrayIcon),
#endif
      mStartTime(QDateTime::currentMSecsSinceEpoch())
{
#ifdef QHttpEngine_FOUND
    connect(&mApiServer, &ApiServer::error, this, &Application::notifyError);
    connect(&mApiServer, &ApiServer::itemsQueued, this, &Application::onItemsQueued);
#endif

    connect(&mDeviceModel, &DeviceModel::rowsInserted, this, &Application::notifyDevicesAdded);
    connect(&mDeviceModel, &DeviceModel::rowsAboutToBeRemoved, this, &Application::notifyDevicesRemoved);
    connect(&mTransferModel, &TransferModel::dataChanged, this, &Application::notifyTransfersChanged);
    connect(&mTransferModel, &TransferModel::error, this, &Application::notifyError);
    connect(&mTransferServer, &TransferServer::error, this, &Application::notifyError);
    connect(&mTransferServer, &TransferServer::newTransfer, &mTransferModel, &TransferModel::addReceiver);
    connect(&mTransferWindow, &TransferWindow::sendDirectory, this, &Application::sendDirectory);
    connect(&mTransferWindow, &TransferWindow::sendFiles, this, &Application::sendFiles);

    mIcon->addAction(tr("Send Files..."), this, SLOT(sendFiles()));
    mIcon->addAction(tr("Send Directory..."), this, SLOT(sendDirectory()));
    mIcon->addSeparator();
    mIcon->addAction(tr("View Transfers..."), this, SLOT(onViewTransfers()));
    mIcon->addSeparator();
    mIcon->addAction(tr("Settings"), this, SLOT(onOpenSettings()));
    mIcon->addSeparator();
    mIcon->addAction(tr("About..."), this, SLOT(onOpenAbout()));
    mIcon->addAction(tr("About Qt..."), this, SLOT(onOpenAboutQt()));
    mIcon->addSeparator();
    mIcon->addAction(tr("Exit"), QApplication::instance(), SLOT(quit()));

    // Start the API server (if enabled) and the transfer server

#ifdef QHttpEngine_FOUND
    mApiServer.start();
#endif

    mTransferServer.start();
}

Application::~Application()
{
    delete mIcon;
}

void Application::notifyError(const QString &message)
{
    QMessageBox::critical(nullptr, tr("Error"), message);
}

void Application::notifyDevicesAdded(const QModelIndex &, int first, int last)
{
    // Only display notifications if current uptime exceeds the broadcast timeout
    if (QDateTime::currentMSecsSinceEpoch() - mStartTime > Settings::instance()->get(Settings::Key::BroadcastTimeout).toInt()) {
        for (int row = first; row <= last; ++row) {
            mIcon->showMessage(tr("%1 has joined.").arg(
                mDeviceModel.data(mDeviceModel.index(row, 0), DeviceModel::NameRole).toString()
            ));
        }
    }
}

void Application::notifyDevicesRemoved(const QModelIndex &, int first, int last)
{
    for (int row = first; row <= last; ++row) {
        mIcon->showMessage(tr("%1 has left.").arg(
            mDeviceModel.data(mDeviceModel.index(row, 0), DeviceModel::NameRole).toString()
        ));
    }
}

void Application::notifyTransfersChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
    // Display notifications if one of the following happens:
    // - the device name is known (receiving only)
    // - transfer fails or succeeds
    for (int row = topLeft.row(); row <= bottomRight.row(); ++row) {

        QModelIndex index = mTransferModel.index(row, 0);
        if (roles.contains(TransferModel::DeviceNameRole)) {

            mIcon->showMessage(tr("Receiving transfer from %1.").arg(
                index.data(TransferModel::DeviceNameRole).toString()
            ));
        } else if (roles.contains(TransferModel::StateRole)) {

            switch (index.data(TransferModel::StateRole).toInt()) {
            case TransferModel::Failed:
                mIcon->showMessage(tr("Transfer with %1 failed.").arg(
                    index.data(TransferModel::DeviceNameRole).toString()
                ));
                break;
            case TransferModel::Succeeded:
                mIcon->showMessage(tr("Transfer with %1 succeeded.").arg(
                    index.data(TransferModel::DeviceNameRole).toString()
                ));
                break;
            }
        }
    }
}

void Application::onItemsQueued(const QStringList &items)
{
    Bundle *bundle = new Bundle;

    foreach(QString item, items) {
        bundle->addItem(item);
    }

    sendBundle(bundle);
}

void Application::sendFiles()
{
    QStringList filenames(QFileDialog::getOpenFileNames(nullptr, tr("Select Files")));

    if (filenames.count()) {
        Bundle *bundle = new Bundle;

        foreach (QString filename, filenames) {
            bundle->addFile(filename);
        }

        sendBundle(bundle);
    }
}

void Application::sendDirectory()
{
    QString path(QFileDialog::getExistingDirectory(nullptr, tr("Select Directory")));

    if (!path.isNull()) {
        Bundle *bundle = new Bundle;

        bundle->addDirectory(path);
        sendBundle(bundle);
    }
}

void Application::onViewTransfers()
{
    mTransferWindow.show();
    mTransferWindow.setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            mTransferWindow.size(),
            QApplication::desktop()->availableGeometry()
        )
    );
    mTransferWindow.raise();
}

void Application::onOpenSettings()
{
    SettingsDialog().exec();
}

void Application::onOpenAbout()
{
    AboutDialog().exec();
}

void Application::onOpenAboutQt()
{
    QMessageBox::aboutQt(nullptr);
}

void Application::sendBundle(const Bundle *bundle)
{
    QModelIndex index = DeviceDialog::getDevice(&mDeviceModel);
    if (index.isValid()) {

        // Obtain the information needed to connect to the device
        QString deviceName = index.data(DeviceModel::NameRole).value<QString>();
        QHostAddress address = index.data(DeviceModel::AddressRole).value<QHostAddress>();
        quint16 port = index.data(DeviceModel::PortRole).value<quint16>();

        mTransferModel.addSender(deviceName, address, port, bundle);
        mTransferWindow.show();
    }
}
