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

#include <QString>
#include <QPixmap>
#include <QFont>

#include "config.h"
#include "aboutdialog.h"
#include "ui_aboutdialog.h"

const QString AboutDialog::CREDITS = QStringLiteral(
"<html><head><meta name=\"qrichtext\" content=\"1\" />"
"<style type=\"text/css\">p { white-space: pre-wrap; margin-top:0px; margin-bottom:0px; "
"margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; } "
"li { white-space: pre-wrap; } span { text-decoration: underline; color:#0000ff; }</style>"
"</head><body>"
"<p>          Created by <a href=\"mailto:nathan@quickmediasolutions.com\">"
"<span>Nathan Osman</span></a></p>"
"<p>                                <a href=\"mailto:daniel.samrocha@gmail.com\">"
"<span>Daniel San</span></a></p>"
"<p>Documented by <a href=\"mailto:nathan@quickmediasolutions.com\">"
"<span>Nathan Osman</span></a></p>"
"<p>     Translated by <a href=\"mailto:nathan@quickmediasolutions.com\">"
"<span>Nathan Osman</span></a></p>"
"</body></html>"
);

const QString AboutDialog::LICENSE = QStringLiteral(
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
"IN THE SOFTWARE.");

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

    ui->lblVersion->setText(PROJECT_VERSION);
    ui->lblDescription->setText(PROJECT_DESCRIPTION);

    QPixmap pixmap(":/img/logo.svg");
    ui->lblLogo->setPixmap(pixmap.scaled(128, 128));

    QFont font = ui->lblProjectTitle->font();
    font.setBold(true);
    ui->lblProjectTitle->setFont(font);
    ui->lblProjectTitle->setText(PROJECT_TITLE);

    QString domainTxt("<html><head/><body><a href=\"http://%1\">Web page</a></body></html>");
    ui->lblDomain->setText(domainTxt.arg(PROJECT_DOMAIN));

    ui->lblCopying->setText(QString("Copyright (c) 2015 %1").arg(PROJECT_AUTHOR));

    ui->txtBws->setVisible(false);

    connect(ui->btnCredits, &QPushButton::clicked,
            this, &AboutDialog::onCreditsOrLicenceChecked);
    connect(ui->btnLicense, &QPushButton::clicked,
            this, &AboutDialog::onCreditsOrLicenceChecked);
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::onCreditsOrLicenceChecked(bool checked)
{
    if (sender()->objectName() == "btnLicense") {
        ui->txtBws->setText(LICENSE);
        ui->btnCredits->setChecked(false);
    } else if (sender()->objectName() == "btnCredits") {
        ui->txtBws->setText(CREDITS);
        ui->btnLicense->setChecked(false);
    }
    ui->txtBws->setVisible(checked);
    ui->wdtContent->setVisible(!checked);
}
