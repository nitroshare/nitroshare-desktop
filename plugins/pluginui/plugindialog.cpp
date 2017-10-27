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
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QMessageBox>
#include <QModelIndexList>
#include <QSpacerItem>
#include <QVBoxLayout>

#include <nitroshare/application.h>
#include <nitroshare/plugin.h>
#include <nitroshare/pluginmodel.h>

#include "plugindialog.h"

const QString ThisPlugin = "pluginui";

PluginDialog::PluginDialog(Application *application)
    : mApplication(application),
      mTableView(new QTableView),
      mLoadButton(new QPushButton(tr("Load"))),
      mUnloadButton(new QPushButton(tr("Unload")))
{
    setWindowTitle(tr("Plugins"));
    resize(800, 300);

    mModel.setSourceModel(application->pluginModel());

    mTableView->setModel(&mModel);
    mTableView->horizontalHeader()->setDefaultSectionSize(160);
    mTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    mTableView->horizontalHeader()->setStretchLastSection(true);
    mTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    mTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    mTableView->verticalHeader()->setVisible(false);

    connect(mTableView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &PluginDialog::onSelectionChanged);

    mLoadButton->setEnabled(false);
    connect(mLoadButton, &QPushButton::clicked, this, &PluginDialog::onLoad);

    mUnloadButton->setEnabled(false);
    connect(mUnloadButton, &QPushButton::clicked, this, &PluginDialog::onUnload);

    QVBoxLayout *vboxLayout = new QVBoxLayout;
    vboxLayout->addWidget(mLoadButton);
    vboxLayout->addWidget(mUnloadButton);
    vboxLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));

    QHBoxLayout *hboxLayout = new QHBoxLayout;
    hboxLayout->addWidget(mTableView);
    hboxLayout->addLayout(vboxLayout);
    setLayout(hboxLayout);
}

void PluginDialog::onSelectionChanged()
{
    Plugin *plugin = currentPlugin();

    mLoadButton->setEnabled(plugin && !plugin->isLoaded());
    mUnloadButton->setEnabled(plugin && plugin->isLoaded());
}

void PluginDialog::onLoad()
{
    if (!mApplication->pluginModel()->load(currentPlugin())) {
        QMessageBox::critical(
            this,
            tr("Error"),
            tr("Unable to load the specified plugin or its dependencies.")
        );
    }
    mTableView->selectionModel()->clear();
}

void PluginDialog::onUnload()
{
    Plugin *plugin = currentPlugin();
    if (plugin->name() == ThisPlugin) {
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
    mApplication->pluginModel()->unload(plugin);
    if (plugin->name() != ThisPlugin) {
        mTableView->selectionModel()->clear();
    }
}

Plugin *PluginDialog::currentPlugin() const
{
    QModelIndexList selection(mTableView->selectionModel()->selectedIndexes());
    return selection.size() ? selection.at(0).data(Qt::UserRole).value<Plugin*>() : nullptr;
}
