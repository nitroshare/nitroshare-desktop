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

#include <QHBoxLayout>
#include <QInputDialog>
#include <QLabel>
#include <QVBoxLayout>

#include <nitroshare/setting.h>

#include "stringlistsettingwidget.h"

StringListSettingWidget::StringListSettingWidget(Setting *setting)
    : mListWidget(new QListWidget),
      mRemoveButton(new QPushButton(tr("Remove")))
{
    connect(mListWidget, &QListWidget::itemSelectionChanged, this, &StringListSettingWidget::onItemSelectionChanged);
    connect(mRemoveButton, &QPushButton::clicked, this, &StringListSettingWidget::onRemoveClicked);

    mListWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    mRemoveButton->setEnabled(false);

    QLabel *label = new QLabel(setting->title());

    QPushButton *addButton = new QPushButton(tr("Add..."));
    connect(addButton, &QPushButton::clicked, this, &StringListSettingWidget::onAddClicked);

    QHBoxLayout *hboxLayout = new QHBoxLayout;
    hboxLayout->addWidget(addButton);
    hboxLayout->addWidget(mRemoveButton);

    QVBoxLayout *vboxLayout = new QVBoxLayout;
    vboxLayout->setMargin(0);
    vboxLayout->addWidget(label);
    vboxLayout->addWidget(mListWidget);
    vboxLayout->addLayout(hboxLayout);
    setLayout(vboxLayout);
}

QVariant StringListSettingWidget::value() const
{
    QStringList items;
    for (int i = 0; i < mListWidget->count(); ++i) {
        items.append(mListWidget->item(i)->text());
    }
    return items;
}

void StringListSettingWidget::setValue(const QVariant &value)
{
    mListWidget->clear();
    mListWidget->addItems(value.toStringList());
}

void StringListSettingWidget::onAddClicked()
{
    QString newItem = QInputDialog::getText(nullptr, tr("Add Item"), tr("Please enter a value for the new item:"));
    if (!newItem.isNull()) {
        mListWidget->addItem(newItem);
    }
}

void StringListSettingWidget::onRemoveClicked()
{
    foreach (auto item, mListWidget->selectedItems()) {
        delete mListWidget->takeItem(mListWidget->row(item));
    }
}

void StringListSettingWidget::onItemSelectionChanged()
{
    mRemoveButton->setEnabled(mListWidget->selectedItems().count());
}
