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

#include "../util/platform.h"
#include "config.h"
#include "settings.h"
#include "settingsdialog.h"

SettingsDialog::SettingsDialog()
    : mAutoStart(Platform::autoStart())
{
    setupUi(this);

#ifndef QHttpEngine_FOUND
    // Disable the local API option if built without QHttpEngine
    localApiCheckBox->setEnabled(false);
#endif

    // Toggle suppress setting with the value of quarantine
    connect(quarantineCheckBox, &QCheckBox::toggled, [this](bool checked) {
        quarantineSuppressCheckBox->setEnabled(checked);
    });

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
#ifdef QHttpEngine_FOUND
    settings->set(Settings::Key::LocalAPI, localApiCheckBox->isChecked());
#endif
    settings->set(Settings::Key::BehaviorReceive, receiveFilesCheckBox->isChecked());
    settings->set(Settings::Key::BehaviorQuarantine, quarantineCheckBox->isChecked());
    settings->set(Settings::Key::BehaviorQuarantineSuppressPrompt, quarantineSuppressCheckBox->isChecked());
    settings->set(Settings::Key::BehaviorOverwrite, overwriteCheckBox->isChecked());

    // Settings in the security tab
    settings->set(Settings::Key::TLS, tlsCheckBox->isChecked());
    settings->set(Settings::Key::TLSCACertificate, caCertificateEdit->text());
    settings->set(Settings::Key::TLSCertificate, certificateEdit->text());
    settings->set(Settings::Key::TLSPrivateKey, privateKeyEdit->text());
    settings->set(Settings::Key::TLSPrivateKeyPassphrase, privateKeyPassphraseEdit->text());

    // Settings in the transfer section
    settings->set(Settings::Key::TransferPort, transferPortSpinBox->value());
    settings->set(Settings::Key::TransferBuffer, transferBufferSpinBox->value() * Settings::Constant::KiB);

    // Settings in the broadcast section
    settings->set(Settings::Key::BroadcastPort, broadcastPortSpinBox->value());
    settings->set(Settings::Key::BroadcastTimeout, broadcastTimeoutSpinBox->value() * Settings::Constant::Second);
    settings->set(Settings::Key::BroadcastInterval, broadcastIntervalSpinBox->value() * Settings::Constant::Second);

    // Enable / disable auto start if changed
    if (mAutoStart != autoStartCheckBox->isChecked()) {
        if (!Platform::setAutoStart(autoStartCheckBox->isChecked())) {
            QMessageBox::critical(nullptr, tr("Error"), tr("Cannot enable or disable auto-start."));
        }
    }

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
    if (response == QMessageBox::Yes) {
        Settings::instance()->reset();
        reload();
    }
}

void SettingsDialog::onBrowseButtonClicked()
{
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    QLineEdit *edit = nullptr;
    bool selectDir = false;

    if (btn == transferDirectoryButton) {
        edit = transferDirectoryEdit;
        selectDir = true;
    } else if (btn == caCertificateButton) {
        edit = caCertificateEdit;
    } else if (btn == certificateButton) {
        edit = certificateEdit;
    } else if (btn == privateKeyButton) {
        edit = privateKeyEdit;
    } else {
        return;
    }

    QString path = selectDir ?
            QFileDialog::getExistingDirectory(this, tr("Select Directory"), edit->text()) :
            QFileDialog::getOpenFileName(this, tr("Select File"), edit->text());
    if (!path.isNull()) {
        edit->setText(path);
    }
}

void SettingsDialog::reload()
{
    Settings *settings = Settings::instance();

    // General tab
    deviceNameEdit->setText(settings->get(Settings::Key::DeviceName).toString());
    transferDirectoryEdit->setText(settings->get(Settings::Key::TransferDirectory).toString());
#ifdef QHttpEngine_FOUND
    localApiCheckBox->setChecked(settings->get(Settings::Key::LocalAPI).toBool());
#endif
    autoStartCheckBox->setChecked(mAutoStart);
    receiveFilesCheckBox->setChecked(settings->get(Settings::Key::BehaviorReceive).toBool());
    quarantineCheckBox->setChecked(settings->get(Settings::Key::BehaviorQuarantine).toBool());
    quarantineSuppressCheckBox->setChecked(settings->get(Settings::Key::BehaviorQuarantineSuppressPrompt).toBool());
    overwriteCheckBox->setChecked(settings->get(Settings::Key::BehaviorOverwrite).toBool());

    // Security tab
    tlsCheckBox->setChecked(settings->get(Settings::Key::TLS).toBool());
    caCertificateEdit->setText(settings->get(Settings::Key::TLSCACertificate).toString());
    certificateEdit->setText(settings->get(Settings::Key::TLSCertificate).toString());
    privateKeyEdit->setText(settings->get(Settings::Key::TLSPrivateKey).toString());
    privateKeyPassphraseEdit->setText(settings->get(Settings::Key::TLSPrivateKeyPassphrase).toString());

    // Conditionally enable the security items if TLS is checked
    emit tlsCheckBox->clicked(tlsCheckBox->isChecked());

    // Transfer section
    transferPortSpinBox->setValue(settings->get(Settings::Key::TransferPort).toLongLong());
    transferBufferSpinBox->setValue(settings->get(Settings::Key::TransferBuffer).toInt() / Settings::Constant::KiB);

    // Broadcast section
    broadcastPortSpinBox->setValue(settings->get(Settings::Key::BroadcastPort).toLongLong());
    broadcastTimeoutSpinBox->setValue(settings->get(Settings::Key::BroadcastTimeout).toInt() / Settings::Constant::Second);
    broadcastIntervalSpinBox->setValue(settings->get(Settings::Key::BroadcastInterval).toInt() / Settings::Constant::Second);
}
