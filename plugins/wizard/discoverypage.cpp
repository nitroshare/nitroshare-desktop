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

#include <QCheckBox>
#include <QLabel>
#include <QVBoxLayout>

#include "discoverypage.h"

DiscoveryPage::DiscoveryPage()
{
    setTitle(tr("Discovery Setup"));
    setSubTitle(tr("Configure settings for device discovery"));

    QLabel *descriptionLabel = new QLabel(tr(
        "NitroShare uses two methods to discover other devices on the "
        "network: IPv4 broadcast and mDNS (multicast DNS).\n\n"
        "mDNS is used by NitroShare 0.3.4+ and is a requirement for using the "
        "Android app. Enabling it is highly recommended.\n\n"
        "IPv4 broadcast is used by older versions of NitroShare and is less "
        "efficient at discovery. It only needs to be enabled if you are using "
        "an older version of the application on the network."
    ));
    descriptionLabel->setWordWrap(true);

    QCheckBox *mdnsEnabledCheckBox = new QCheckBox(tr("Enable mDNS discovery (recommended)"));
    mdnsEnabledCheckBox->setChecked(true);
    registerField("mdnsEnabled", mdnsEnabledCheckBox);

    QCheckBox *broadcastEnabledCheckBox = new QCheckBox(tr("Enable broadcast discovery"));
    registerField("broadcastEnabled", broadcastEnabledCheckBox);

    QVBoxLayout *vboxLayout = new QVBoxLayout;
    vboxLayout->addWidget(descriptionLabel);
    vboxLayout->addWidget(mdnsEnabledCheckBox);
    vboxLayout->addWidget(broadcastEnabledCheckBox);
    setLayout(vboxLayout);
}
