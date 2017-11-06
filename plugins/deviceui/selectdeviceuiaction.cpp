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

#include "selectdevicedialog.h"
#include "selectdeviceuiaction.h"

SelectDeviceUiAction::SelectDeviceUiAction(Application *application)
    : mApplication(application)
{
}

QString SelectDeviceUiAction::name() const
{
    return "selectdeviceui";
}

bool SelectDeviceUiAction::api() const
{
    return true;
}

QString SelectDeviceUiAction::title() const
{
    return tr("show device selection dialog");
}

QString SelectDeviceUiAction::description() const
{
    return tr(
        "Show a dialog for selecting a device. "
        "This action takes no parameters and returns either boolean false if "
        "no device was selected or an object with two properties:\n\n"
        "\n"
        "- \"device\" (string) UUID of the selected device\n"
        "- \"enumerator\" (string) name of the enumerator for the selected device"
    );
}

QVariant SelectDeviceUiAction::invoke(const QVariantMap &)
{
    SelectDeviceDialog dialog(mApplication);
    if (dialog.exec() == QDialog::Accepted) {
        return QVariantMap{
            { "device", dialog.deviceUuid() },
            { "enumerator", dialog.enumeratorName() }
        };
    }
    return false;
}
