/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Nathan Osman
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

#include <QHeaderView>
#include <QJsonValue>
#include <QLabel>
#include <QMetaObject>
#include <QMetaProperty>
#include <QModelIndexList>
#include <QTableWidgetItem>
#include <QVBoxLayout>

#include <nitroshare/application.h>
#include <nitroshare/device.h>
#include <nitroshare/devicemodel.h>
#include <nitroshare/jsonutil.h>

#include "deviceswidget.h"

DevicesWidget::DevicesWidget(Application *application)
    : mListView(new QListView),
      mTableWidget(new QTableWidget)
{
    QLabel *instructionsLabel = new QLabel(tr(
        "Use this tool to view the devices discovered by enumerators."
    ));

    mListView->setModel(application->deviceModel());

    mTableWidget->horizontalHeader()->setStretchLastSection(true);
    mTableWidget->verticalHeader()->setVisible(false);

    connect(mListView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &DevicesWidget::onSelectionChanged);

    QVBoxLayout *vboxLayout = new QVBoxLayout;
    vboxLayout->addWidget(instructionsLabel);
    vboxLayout->addWidget(mListView);
    vboxLayout->addWidget(new QLabel(tr("Properties:")));
    vboxLayout->addWidget(mTableWidget);
    setLayout(vboxLayout);
}

void DevicesWidget::onSelectionChanged()
{
    // Begin by clearing the list widget
    mTableWidget->clear();

    QModelIndexList selection = mListView->selectionModel()->selectedIndexes();
    if (selection.size()) {

        // Retrieve the device currently selected
        Device *device = selection.at(0).data(Qt::UserRole).value<Device *>();

        mTableWidget->setRowCount(device->metaObject()->propertyCount() - 1);
        mTableWidget->setColumnCount(2);

        mTableWidget->setHorizontalHeaderLabels(QStringList() << tr("Name") << tr("Value"));
        mTableWidget->setColumnWidth(0, 180);

        // Populate the table with the device properties
        for (int i = 1; i < device->metaObject()->propertyCount(); ++i) {
            auto property = device->metaObject()->property(i);
            auto name = property.name();
            auto value = device->property(name);

            // Insert the new row
            mTableWidget->setItem(i - 1, 0, new QTableWidgetItem(name));
            mTableWidget->setItem(i - 1, 1, new QTableWidgetItem(QString(
                JsonUtil::jsonValueToByteArray(QJsonValue::fromVariant(value))
            )));
        }

        mTableWidget->resizeRowsToContents();
    }
}
