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

#ifndef NS_APISERVER_H
#define NS_APISERVER_H

#include <QObject>

class ApiServerPrivate;

/**
 * @brief Server providing a local ApiHandler
 *
 * This class provides an HTTP server that local applications running under
 * the user's account can use to initiate transfers.
 */
class ApiServer : public QObject
{
    Q_OBJECT

public:

    /**
     * @brief Create a new API server
     * @param parent parent QObject
     */
    ApiServer(QObject *parent = nullptr);

Q_SIGNALS:

    /**
     * @brief Indicate an error
     * @param message description of the error
     */
    void error(const QString &message);

    /**
     * @brief Indicate new items to transfer
     * @param list of file and directory paths
     */
    void itemsQueued(const QStringList &items);

public Q_SLOTS:

    /**
     * @brief Start the server
     */
    void start();

private:

    ApiServerPrivate *const d;
};

#endif // NS_APISERVER_H
