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

#include <QFileDialog>
#include <QMessageBox>

#include "settings.h"
#include "settingsdialog.h"

#ifdef BUILD_UPDATECHECKER
#include "../application/updatechecker.h"
#endif

SettingsDialog::SettingsDialog()
{
    setupUi(this);

    // Load the current values into the controls
    reload();

    connect(pshBtnSelectDir, &QPushButton::clicked, this, &SettingsDialog::onBtnSelectDirClicked);
}

void SettingsDialog::accept()
{
    // General tab
    Settings::set(Settings::DeviceName, lnEdtdeviceName->text());

#ifdef BUILD_UPDATECHECKER
    if (chkBoxCheckUpdates->isChecked()) {
        Settings::set(Settings::UpdateInterval, spnBoxUpdateInterval->value() * Settings::Hour);
        emit configureUpdateChecker();
    } else {
        Settings::set(Settings::UpdateInterval, 0);
        UpdateChecker::deleteInstance();
    }
#endif

    // Transfer tab
    Settings::set(Settings::TransferDirectory, lnEdtDirectory->text());
    Settings::set(Settings::TransferPort, spnBoxTransferPort->value());
    Settings::set(Settings::TransferBuffer, spnBoxBuffer->value() * Settings::Kb);

    // Broadcast tab
    Settings::set(Settings::BroadcastPort, spnBoxBroadcastPort->value());
    Settings::set(Settings::BroadcastTimeout, spnBoxBroadcastTimeout->value() * Settings::Second);
    Settings::set(Settings::BroadcastInterval, spnBoxBroadcastInterval->value() * Settings::Second);

    QDialog::accept();
}

void SettingsDialog::onBtnResetClicked()
{
    QMessageBox::StandardButton response = QMessageBox::question(this,
                                                                 "Reset settings",
                                                                 "Are you sure you want to reset "
                                                                 "all settings to their default "
                                                                 "values? This cannot be undone.");
    if (response == QMessageBox::Yes) {
        Settings::reset();
#ifdef BUILD_UPDATECHECKER
        Q_EMIT configureUpdateChecker();
#endif

        // Reload the current values
        reload();
    }
}

void SettingsDialog::onBtnSelectDirClicked()
{
    QString path(QFileDialog::getExistingDirectory(this, tr("Select Directory")));

    if(!path.isNull()) {
        lnEdtDirectory->setText(path);
    }
}

void SettingsDialog::reload()
{
    // General tab
    lnEdtdeviceName->setText(Settings::get(Settings::DeviceName).toString());

#ifdef BUILD_UPDATECHECKER
    int updateInterval = Settings::get(Settings::UpdateInterval).toInt();
    bool checkForUpdates = (updateInterval != 0);
    lblUpdateInterval->setEnabled(checkForUpdates);
    spnBoxUpdateInterval->setEnabled(checkForUpdates);
    chkBoxCheckUpdates->setChecked(checkForUpdates);
    spnBoxUpdateInterval->setValue(updateInterval / Settings::Hour);
#else
    chkBoxCheckUpdates->setVisible(false);
    lblUpdateInterval->setVisible(false);
    spnBoxUpdateInterval->setVisible(false);
#endif

    // Transfer section
    spnBoxBuffer->setValue(Settings::get(Settings::TransferBuffer).toInt() / Settings::Kb);
    lnEdtDirectory->setText(Settings::get(Settings::TransferDirectory).toString());
    spnBoxTransferPort->setValue(Settings::get(Settings::TransferPort).toLongLong());

    // Broadcast section
    spnBoxBroadcastPort->setValue(Settings::get(Settings::BroadcastPort).toLongLong());
    spnBoxBroadcastTimeout->setValue(Settings::get(Settings::BroadcastTimeout).toInt() / Settings::Second);
    spnBoxBroadcastInterval->setValue(Settings::get(Settings::BroadcastInterval).toInt() / Settings::Second);
}
