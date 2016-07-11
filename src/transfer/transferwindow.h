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

#ifndef NS_TRANSFERWINDOW_H
#define NS_TRANSFERWINDOW_H

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMainWindow>

#include "config.h"
#include "transfermodel.h"
#include "ui_transferwindow.h"

#ifdef Qt5WinExtras_FOUND
#include <QWinTaskbarButton>
#endif

/**
 * @brief Displays a list of all transfers in the model
 *
 * Given a TransferModel, this class will present a window that allows the
 * user to view the transfers that are currently in progress. In addition,
 * the user can cancel transfers in progress and restart failed transfers.
 *
 * The window listens for changes to the model and inserts the appropriate
 * widgets into cells in the table. The progress bar is updated as necessary.
 */
class TransferWindow : public QMainWindow, public Ui::TransferWindow
{
    Q_OBJECT

public:

    explicit TransferWindow(TransferModel *model);

Q_SIGNALS:

    void sendFiles();
    void sendDirectory();

    void itemsQueued(const QStringList &items);

public Q_SLOTS:

    void onRowsInserted(const QModelIndex &parent, int first, int last);
    void onDataChanged(const QModelIndex & topLeft, const QModelIndex & bottomRight, const QVector<int> &roles);

private:

    virtual void showEvent(QShowEvent *);
    virtual void hideEvent(QHideEvent *);

    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

    void updateProgressBar(int row);
    void updateButton(int row);

#ifdef Q_OS_MAC
    void setForeground(bool foreground);
#endif

    TransferModel *const mModel;

#ifdef Qt5WinExtras_FOUND
    QWinTaskbarButton *mTaskbarButton;
#endif

#ifdef Q_OS_LINUX
    void *mLauncherEntry;
#endif
};

#endif // NS_TRANSFERDIALOG_H
