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

#ifndef NS_SETTINGS_H
#define NS_SETTINGS_H

#include <QSettings>

class Settings : public QSettings
{
    Q_OBJECT

public:

    enum Key {
        DeviceTimeout,
        InterfaceMonitorInterval,
        MulticastAddress,
        MulticastPort,
        Name,
        TransferPort,
        UUID
    };

    template <class T>
    static T get(Key key) {
        return loadValue(key).value<T>();
    }

    static void set(Key key, const QVariant &value) { storeValue(key, value, false); }

    static Settings * instance();

signals:

    void settingChanged(Key key);

private:

    static QVariant loadValue(Key key);
    static void storeValue(Key key, const QVariant &value, bool initializing);
};

#endif // NS_SETTINGS_H
