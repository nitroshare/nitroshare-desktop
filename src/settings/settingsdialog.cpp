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
    Settings *settings = Settings::instance();
    settings->beginSet();

    // Settings in the general tab
    settings->set(Settings::Key::DeviceName, deviceNameEdit->text());
    settings->set(Settings::Key::TransferDirectory, transferDirectoryEdit->text());

#ifdef BUILD_UPDATECHECKER
    settings->set(Settings::Key::UpdateInterval, updateCheckbox->isChecked() ? updateIntervalSpinBox->value() * Settings::Constant::Hour : 0);
#endif

    // Settings in the transfer section
    settings->set(Settings::Key::TransferPort, transferPortSpinBox->value());
    settings->set(Settings::Key::TransferBuffer, transferBufferSpinBox->value() * Settings::Constant::KiB);

    // Settings in the broadcast section
    settings->set(Settings::Key::BroadcastPort, broadcastPortSpinBox->value());
    settings->set(Settings::Key::BroadcastTimeout, broadcastTimeoutSpinBox->value() * Settings::Constant::Second);
    settings->set(Settings::Key::BroadcastInterval, broadcastIntervalSpinBox->value() * Settings::Constant::Second);

    settings->endSet();
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
        Settings::instance()->reset();
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
    Settings *settings = Settings::instance();

    // General tab
    deviceNameEdit->setText(settings->get(Settings::Key::DeviceName).toString());
    transferDirectoryEdit->setText(settings->get(Settings::Key::TransferDirectory).toString());

#ifdef BUILD_UPDATECHECKER
    const int updateInterval = settings->get(Settings::Key::UpdateInterval).toInt();
    updateCheckbox->setChecked(updateInterval);
    updateIntervalSpinBox->setEnabled(updateInterval);
    updateIntervalSpinBox->setValue(updateInterval / Settings::Constant::Hour);
#endif

    // Transfer section
    transferPortSpinBox->setValue(settings->get(Settings::Key::TransferPort).toLongLong());
    transferBufferSpinBox->setValue(settings->get(Settings::Key::TransferBuffer).toInt() / Settings::Constant::KiB);

    // Broadcast section
    broadcastPortSpinBox->setValue(settings->get(Settings::Key::BroadcastPort).toLongLong());
    broadcastTimeoutSpinBox->setValue(settings->get(Settings::Key::BroadcastTimeout).toInt() / Settings::Constant::Second);
    broadcastIntervalSpinBox->setValue(settings->get(Settings::Key::BroadcastInterval).toInt() / Settings::Constant::Second);
}
