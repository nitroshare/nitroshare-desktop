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

#ifndef STATICENUMERATOR_H
#define STATICENUMERATOR_H

#include <QMap>

#include <nitroshare/category.h>
#include <nitroshare/deviceenumerator.h>
#include <nitroshare/setting.h>

class Application;

class StaticDevice;

class StaticEnumerator : public DeviceEnumerator
{
    Q_OBJECT

public:

    explicit StaticEnumerator(Application *application);
    virtual ~StaticEnumerator();

    virtual QString name() const;

private slots:

    void onSettingsChanged(const QStringList &keys);

private:

    Application *mApplication;

    Category mStaticCategory;
    Setting mStaticDevices;

    QMap<QString, StaticDevice*> mDevices;
};

#endif // STATICENUMERATOR_H
