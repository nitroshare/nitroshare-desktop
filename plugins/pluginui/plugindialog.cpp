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

#include <QHeaderView>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

#include <nitroshare/application.h>
#include <nitroshare/pluginmodel.h>

#include "plugindialog.h"

PluginDialog::PluginDialog(Application *application)
    : mApplication(application),
      mTableView(new QTableView)
{
    setWindowTitle(tr("Plugins"));
    resize(800, 300);

    connect(&mModel, &PluginProxyModel::rowsInserted, this, &PluginDialog::onRowsInserted);

    mModel.setSourceModel(application->pluginModel());

    mTableView->setModel(&mModel);
    mTableView->horizontalHeader()->setDefaultSectionSize(160);
    mTableView->horizontalHeader()->setStretchLastSection(true);
    mTableView->horizontalHeader()->resizeSection(PluginProxyModel::VersionColumn, 80);
    mTableView->setSelectionMode(QAbstractItemView::NoSelection);
    mTableView->setStyleSheet("QTableView::item { padding: 4px; }");
    mTableView->verticalHeader()->setVisible(false);

    for (int row = 0; row < mModel.rowCount(); ++row) {
        addButtons(row);
    }

    QVBoxLayout *vboxLayout = new QVBoxLayout;
    vboxLayout->addWidget(mTableView);
    setLayout(vboxLayout);
}

void PluginDialog::onRowsInserted(const QModelIndex &parent, int start, int end)
{
    for (int row = start; row <= end; ++row) {
        addButtons(row);
    }
}

void PluginDialog::addButtons(int row)
{
    QPushButton *pushButton = new QPushButton(tr("Unload"));
    connect(pushButton, &QPushButton::clicked, [this, row]() {
        QString name = mModel.data(mModel.index(row, 0), PluginModel::NameRole).toString();
        if (name == "pluginui") {
            int response = QMessageBox::warning(
                this,
                tr("Warning"),
                tr("Unloading the pluginui plugin will close this dialog and make further changes impossible. Are you sure you want to unload this plugin?"),
                QMessageBox::Yes | QMessageBox::No
            );
            if (response == QMessageBox::No) {
                return;
            }
        }
        mApplication->pluginModel()->unloadPlugin(name);
    });
    mTableView->setIndexWidget(mModel.index(row, PluginProxyModel::ActionsColumn), pushButton);
}
