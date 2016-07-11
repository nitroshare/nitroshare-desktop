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

#ifndef NS_BUNDLE_H
#define NS_BUNDLE_H

#include <QObject>

class BundlePrivate;

/**
 * @brief Collection of items for sending in a transfer
 *
 * A bundle maintains the file and directory metadata required to send files
 * in a transfer. If a directory is added, its contents are added recursively
 * as well. As items are added, their size is added to the total.
 */
class Bundle : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int count READ count)
    Q_PROPERTY(qint64 totalSize READ totalSize)

public:

    /**
     * @brief Create an empty bundle
     * @param parent parent QObject
     */
    explicit Bundle(QObject *parent = nullptr);

    /**
     * @brief Destroy the bundle
     */
    virtual ~Bundle();

    /**
     * @brief Retrieve the number of items in the bundle
     * @return item count
     */
    int count() const;

    /**
     * @brief Retrieve the total size of the items in the bundle
     * @return size in bytes
     */
    qint64 totalSize() const;

public Q_SLOTS:

    /**
     * @brief Add an item regardless of type
     * @param path absolute path to item
     */
    void addItem(const QString &path);

    /**
     * @brief Add a file to the bundle
     * @param filename name of the file to add
     * @return true if the file was added
     */
    void addFile(const QString &filename);

    /**
     * @brief Add a directory and its contents to the bundle
     * @param directory name of the directory to add
     * @return true if the directory was added
     */
    void addDirectory(const QString &directory);

private:

    BundlePrivate *const d;
    friend class TransferSender;
};

#endif // NS_BUNDLE_H
