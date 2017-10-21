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

#include <QFrame>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSpacerItem>
#include <QTableView>
#include <QVBoxLayout>
#include <QWidget>

#include <nitroshare/application.h>
#include <nitroshare/transfermodel.h>

#include "transferwindow.h"

TransferWindow::TransferWindow(Application *application)
{
    setWindowTitle(tr("Transfers"));
    resize(640, 200);

    QTableView *tableView = new QTableView;
    tableView->setModel(application->transferModel());

    QPushButton *viewFiles = new QPushButton(tr("View Files..."));
    QPushButton *clear = new QPushButton(tr("Clear"));
    connect(clear, &QPushButton::clicked, []() {
        //...
    });

    QFrame *hline = new QFrame;
    hline->setFrameShape(QFrame::HLine);
    hline->setFrameShadow(QFrame::Sunken);

    QVBoxLayout *vboxLayout = new QVBoxLayout;
    vboxLayout->addWidget(viewFiles);
    vboxLayout->addWidget(hline);
    vboxLayout->addWidget(clear);
    vboxLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));

    QHBoxLayout *hboxLayout = new QHBoxLayout;
    hboxLayout->addWidget(tableView);
    hboxLayout->addLayout(vboxLayout);

    QWidget *widget = new QWidget;
    widget->setLayout(hboxLayout);
    setCentralWidget(widget);
}
