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

#include <QPixmap>
#include <QFont>

#include "aboutdialog.h"
#include "config.h"
#include "ui_aboutdialog.h"

const QString Credits =
    "<html><head><meta name='qrichtext' content='1'/></head><body>"
    "<table><tr><td>Created by:</td>"
    "<td><a href='mailto:nathan@quickmediasolutions.com'>Nathan Osman</a></td></tr>"
    "<tr><td></td><td><a href='mailto:daniel.samrocha@gmail.com'>Daniel San</a>"
    "</td></tr></table></body></html>";

const QString License =
    "The MIT License (MIT)\n\n"

    "Permission is hereby granted, free of charge, to any person obtaining a copy"
    "of this software and associated documentation files (the \"Software\"), to"
    "deal in the Software without restriction, including without limitation the"
    "rights to use, copy, modify, merge, publish, distribute, sublicense, and/or"
    "sell copies of the Software, and to permit persons to whom the Software is"
    "furnished to do so, subject to the following conditions:\n\n"

    "The above copyright notice and this permission notice shall be included in"
    "all copies or substantial portions of the Software.\n\n"

    "THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR"
    "IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,"
    "FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE"
    "AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER"
    "LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING"
    "FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS"
    "IN THE SOFTWARE.";

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent),
      ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

    ui->lblLogo->setPixmap(QPixmap(":/img/logo.svg").scaled(128, 128));

    QFont font = ui->lblProjectTitle->font();
    font.setBold(true);
    ui->lblProjectTitle->setFont(font);
    ui->lblProjectTitle->setText(PROJECT_TITLE);

    ui->lblVersion->setText(QString("Version %1").arg(PROJECT_VERSION));
    ui->lblDescription->setText(PROJECT_DESCRIPTION);

    QString websiteTxt("<html><head/><body><a href=\"%1\">%1</a></body></html>");
    ui->lblWebsite->setText(websiteTxt.arg(PROJECT_WEBSITE));
    ui->lblCopyright->setText(QString("Copyright (c) 2015 - %1").arg(PROJECT_AUTHOR));

    ui->textBrowser->hide();
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::onCreditsOrLicenceClicked(bool checked)
{
    if (sender()->objectName() == "btnLicense") {
        ui->textBrowser->setText(License);
        ui->btnCredits->setChecked(false);
    } else if (sender()->objectName() == "btnCredits") {
        ui->textBrowser->setText(Credits);
        ui->btnLicense->setChecked(false);
    }

    ui->textBrowser->setVisible(checked);
    ui->wdtContent->setVisible(!checked);
}
