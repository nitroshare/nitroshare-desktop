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

#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

#include <nitroshare/setting.h>

#include "pathsettingwidget.h"

PathSettingWidget::PathSettingWidget(Setting *setting)
    : mLineEdit(new QLineEdit),
      mPushButton(new QPushButton(tr("Browse..."))),
      mFile(setting->type() == Setting::FilePath)
{
    connect(mPushButton, &QPushButton::clicked, this, &PathSettingWidget::onClicked);

    QLabel *label = new QLabel(setting->title());

    QHBoxLayout *hboxLayout = new QHBoxLayout;
    hboxLayout->setMargin(0);
    hboxLayout->addWidget(mLineEdit);
    hboxLayout->addWidget(mPushButton);

    QVBoxLayout *vboxLayout = new QVBoxLayout;
    vboxLayout->setMargin(0);
    vboxLayout->setSpacing(0);
    vboxLayout->addWidget(label);
    vboxLayout->addLayout(hboxLayout);

    setLayout(vboxLayout);
}

QVariant PathSettingWidget::value() const
{
    return mLineEdit->text();
}

void PathSettingWidget::setValue(const QVariant &value)
{
    mLineEdit->setText(value.toString());
}

void PathSettingWidget::onClicked()
{
    QString path;

    // Show the appropriate dialog depending on the setting type
    if (mFile) {
        path = QFileDialog::getOpenFileName(nullptr, QString(), path);
    } else {
        path = QFileDialog::getExistingDirectory(nullptr, QString(), path);
    }

    // If the user selected a path, copy it into the line edit
    if (!path.isNull()) {
        mLineEdit->setText(path);
    }
}
