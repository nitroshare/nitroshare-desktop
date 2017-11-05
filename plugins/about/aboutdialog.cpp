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

#include <QDialogButtonBox>
#include <QFont>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QVBoxLayout>

#include <nitroshare/application.h>

#include "aboutdialog.h"

AboutDialog::AboutDialog(Application *application)
{
    setWindowTitle(tr("About NitroShare"));

    QLabel *logoLabel = new QLabel;
    logoLabel->setPixmap(QPixmap(":/about/logo.png").scaled(128, 128));

    QVBoxLayout *logoLayout = new QVBoxLayout;
    logoLayout->addWidget(logoLabel);
    logoLayout->addStretch(1);

    QLabel *titleLabel = new QLabel(tr("NitroShare"));
    QFont titleFont;
    titleFont.setPointSize(16);
    titleLabel->setFont(titleFont);

    QLabel *subtitleLabel = new QLabel(tr("Cross-platform network file transfer application"));
    QLabel *versionLabel = new QLabel(tr("Version: %1").arg(application->version()));
    QLabel *copyrightLabel = new QLabel(tr("Copyright (c) 2017 Nathan Osman"));

    QVBoxLayout *infoLayout = new QVBoxLayout;
    infoLayout->addWidget(titleLabel);
    infoLayout->addWidget(subtitleLabel);
    infoLayout->addWidget(versionLabel);
    infoLayout->addStretch(1);
    infoLayout->addWidget(copyrightLabel);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addLayout(logoLayout);
    mainLayout->addLayout(infoLayout);

    QFrame *frame = new QFrame;
    frame->setFrameShape(QFrame::HLine);
    frame->setFrameShadow(QFrame::Sunken);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &AboutDialog::accept);

    QVBoxLayout *vboxLayout = new QVBoxLayout;
    vboxLayout->addLayout(mainLayout);
    vboxLayout->addWidget(frame);
    vboxLayout->addWidget(buttonBox);
    setLayout(vboxLayout);
}
