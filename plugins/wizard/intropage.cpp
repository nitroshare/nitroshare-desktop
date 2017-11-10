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

#include <QLabel>
#include <QVBoxLayout>

#include "intropage.h"

IntroPage::IntroPage()
{
    setTitle(tr("Introduction"));
    setSubTitle(tr("Welcome to NitroShare"));

    QLabel *label = new QLabel(tr(
        "Thank you for installing NitroShare!\n\nThe application is designed "
        "to make transferring information between devices as simple as "
        "possible. Here are just some of the features included:\n\n"
        "- Automatic peer discovery\n"
        "- Extremely fast file transfers\n"
        "- Compatibility with all major platforms\n\n"
        "This wizard will guide you through the initial setup process to help "
        "ensure NitroShare works exactly the way you want it to."

    ));
    label->setWordWrap(true);

    QVBoxLayout *vboxLayout = new QVBoxLayout;
    vboxLayout->addWidget(label);
    setLayout(vboxLayout);
}
