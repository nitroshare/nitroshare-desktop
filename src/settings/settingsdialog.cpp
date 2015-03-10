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

SettingsDialog::SettingsDialog()
{
    setupUi(this);

#ifndef BUILD_UPDATECHECKER
    // Remove the update controls
    delete updateCheckbox;
    delete updateIntervalLabel;
    delete updateIntervalSpinBox;
#endif

    // Load the current values into the controls
    reload();
}

void SettingsDialog::accept()
{
    // General tab
    Settings::set(Settings::DeviceName, deviceNameEdit->text());
    Settings::set(Settings::TransferDirectory, transferDirectoryEdit->text());

#ifdef BUILD_UPDATECHECKER
    Settings::set(Settings::UpdateInterval, updateCheckbox->isChecked() ? updateIntervalSpinBox->value() * Settings::Hour : 0);
#endif

    // Transfer section
    Settings::set(Settings::TransferPort, transferPortSpinBox->value());
    Settings::set(Settings::TransferBuffer, transferBufferSpinBox->value() * Settings::KiB);

    // Broadcast section
    Settings::set(Settings::BroadcastPort, broadcastPortSpinBox->value());
    Settings::set(Settings::BroadcastTimeout, broadcastTimeoutSpinBox->value() * Settings::Second);
    Settings::set(Settings::BroadcastInterval, broadcastIntervalSpinBox->value() * Settings::Second);

    QDialog::accept();
}

void SettingsDialog::onResetButtonClicked()
{
    // Confirm that the user wants to reset all of the settings
    QMessageBox::StandardButton response = QMessageBox::question(
        this,
        tr("Confirm Reset"),
        tr("Are you sure you want to reset all settings to their default values? This cannot be undone.")
    );

    // Perform the reset and then reload all of the settings
    if(response == QMessageBox::Yes) {
        Settings::reset();
        reload();
    }
}

void SettingsDialog::onTransferDirectoryButtonClicked()
{
    QString path = QFileDialog::getExistingDirectory(this, tr("Select Directory"), transferDirectoryEdit->text());
    if(!path.isNull()) {
        transferDirectoryEdit->setText(path);
    }
}

void SettingsDialog::reload()
{
    // General tab
    deviceNameEdit->setText(Settings::get(Settings::DeviceName).toString());
    transferDirectoryEdit->setText(Settings::get(Settings::TransferDirectory).toString());

#ifdef BUILD_UPDATECHECKER
    const int updateInterval = Settings::get(Settings::UpdateInterval).toInt();
    updateCheckbox->setChecked(updateInterval);
    updateIntervalSpinBox->setEnabled(updateInterval);
    updateIntervalSpinBox->setValue(updateInterval / Settings::Hour);
#endif

    // Transfer section
    transferPortSpinBox->setValue(Settings::get(Settings::TransferPort).toLongLong());
    transferBufferSpinBox->setValue(Settings::get(Settings::TransferBuffer).toInt() / Settings::KiB);

    // Broadcast section
    broadcastPortSpinBox->setValue(Settings::get(Settings::BroadcastPort).toLongLong());
    broadcastTimeoutSpinBox->setValue(Settings::get(Settings::BroadcastTimeout).toInt() / Settings::Second);
    broadcastIntervalSpinBox->setValue(Settings::get(Settings::BroadcastInterval).toInt() / Settings::Second);
}
