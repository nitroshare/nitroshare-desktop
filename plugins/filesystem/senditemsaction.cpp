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

#include <QDir>
#include <QFileInfo>
#include <QStack>

#include <nitroshare/application.h>
#include <nitroshare/bundle.h>
#include <nitroshare/device.h>
#include <nitroshare/devicemodel.h>
#include <nitroshare/transfer.h>
#include <nitroshare/transfermodel.h>

#include "file.h"
#include "senditemsaction.h"

// TODO: make this a configurable setting

const int BlockSize = 65536;

SendItemsAction::SendItemsAction(Application *application)
    : mApplication(application)
{
}

QString SendItemsAction::name() const
{
    return "senditems";
}

bool SendItemsAction::api() const
{
    return true;
}

QString SendItemsAction::title() const
{
    return tr("send items to device");
}

QString SendItemsAction::description() const
{
    return tr(
        "Send a list of files or directories to the specified device. "
        "This action expects three parameters:\n"
        "\n"
        "- \"device\" (string) UUID of the device to send the items to\n"
        "- \"enumerator\" (string) name of the enumerator for the device\n"
        "- \"items\" (array of strings) absolute paths for the items to send\n"
        "\n"
        "The return value will be a boolean indicating if the transfer was created."
    );
}

QVariant SendItemsAction::invoke(const QVariantMap &params)
{
    // Attempt to find the device
    Device *device = mApplication->deviceModel()->findDevice(
        params.value("device").toString(),
        params.value("enumerator").toString()
    );
    if (!device) {
        return false;
    }

    // Create a new bundle with the items that were provided
    Bundle *bundle = createBundle(params.value("items").toStringList());

    // Create the transfer
    mApplication->transferModel()->add(
        new Transfer(mApplication, device, bundle)
    );

    return true;
}

Bundle *SendItemsAction::createBundle(const QStringList &items)
{
    Bundle *bundle = new Bundle;

    foreach (const QString &item, items) {
        QFileInfo info(item);
        if (info.isFile()) {

            // Add the file directly
            bundle->add(new File(info.absolutePath(), info, BlockSize));

        } else if (info.isDir()) {

            // Use a stack to enumerate the content of the directory in a loop
            QDir root(item);
            QStack<QString> stack;

            // Push the root path on the stack and then go up one level so that
            // the relative filenames will include the name of the directory
            stack.push(root.absolutePath());
            root.cdUp();

            // Continue to add items as long as there are items in the stack
            while (stack.count()) {
                auto infoList = QDir(stack.pop()).entryInfoList(
                    QDir::Dirs | QDir::Files | QDir::Hidden | QDir::NoDotAndDotDot | QDir::NoSymLinks
                );
                foreach (auto &info, infoList) {
                    if (info.isDir()) {
                        stack.push(info.absolutePath());
                    } else {
                        bundle->add(new File(root, info, BlockSize));
                    }
                }
            }
        }
    }

    return bundle;
}
