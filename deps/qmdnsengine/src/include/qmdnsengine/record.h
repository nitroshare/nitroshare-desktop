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

#ifndef QMDNSENGINE_RECORD_H
#define QMDNSENGINE_RECORD_H

#include <QByteArray>
#include <QHostAddress>
#include <QMap>

#include <qmdnsengine/bitmap.h>

#include "qmdnsengine_export.h"

namespace QMdnsEngine
{

class QMDNSENGINE_EXPORT RecordPrivate;

/**
 * @brief DNS record
 *
 * This class maintains information for an individual record. Not all record
 * types use every field.
 *
 * For example, to create a TXT record:
 *
 * @code
 * QMdnsEngine::Record record;
 * record.setName("My Service._http._tcp.local.");
 * record.setType(QMdnsEngine::TXT);
 * record.addAttribute("a", "value1");
 * record.addAttribute("b", "value2");
 *
 * message.addRecord(record);
 * @endcode
 */
class QMDNSENGINE_EXPORT Record
{
public:

    /**
     * @brief Create an uninitialized record
     */
    Record();

    /**
     * @brief Create a copy of an existing record
     */
    Record(const Record &other);

    /**
     * @brief Assignment operator
     */
    Record &operator=(const Record &other);

    /**
     * @brief Equality operator
     */
    bool operator==(const Record &other) const;

    /**
     * @brief Inequality operator
     */
    bool operator!=(const Record &other) const;

    /**
     * @brief Destroy the record
     */
    virtual ~Record();

    /**
     * @brief Retrieve the name of the record
     */
    QByteArray name() const;

    /**
     * @brief Set the name of the record
     */
    void setName(const QByteArray &name);

    /**
     * @brief Retrieve the type of the record
     */
    quint16 type() const;

    /**
     * @brief Set the type of the record
     *
     * For convenience, constants for types used by mDNS, such as
     * QMdnsEngine::A or QMdnsEngine::PTR, may be used here.
     */
    void setType(quint16 type);

    /**
     * @brief Determine whether to replace or append to existing records
     *
     * If true, this record replaces all previous records of the same name and
     * type rather than appending to them.
     */
    bool flushCache() const;

    /**
     * @brief Set whether to replace or append to existing records
     */
    void setFlushCache(bool flushCache);

    /**
     * @brief Retrieve the TTL (time to live) for the record
     */
    quint32 ttl() const;

    /**
     * @brief Set the TTL (time to live) for the record
     */
    void setTtl(quint32 ttl);

    /**
     * @brief Retrieve the address for the record
     *
     * This field is used by QMdnsEngine::A and QMdnsEngine::AAAA records.
     */
    QHostAddress address() const;

    /**
     * @brief Set the address for the record
     */
    void setAddress(const QHostAddress &address);

    /**
     * @brief Retrieve the target for the record
     *
     * This field is used by QMdnsEngine::PTR and QMdnsEngine::SRV records.
     */
    QByteArray target() const;

    /**
     * @brief Set the target for the record
     */
    void setTarget(const QByteArray &target);

    /**
     * @brief Retrieve the next domain name
     *
     * This field is used by QMdnsEngine::NSEC records.
     */
    QByteArray nextDomainName() const;

    /**
     * @brief Set the next domain name
     */
    void setNextDomainName(const QByteArray &nextDomainName);

    /**
     * @brief Retrieve the priority for the record
     *
     * This field is used by QMdnsEngine::SRV records.
     */
    quint16 priority() const;

    /**
     * @brief Set the priority for the record
     *
     * Unless more than one QMdnsEngine::SRV record is being sent, this field
     * should be set to 0.
     */
    void setPriority(quint16 priority);

    /**
     * @brief Retrieve the weight of the record
     *
     * This field is used by QMdnsEngine::SRV records.
     */
    quint16 weight() const;

    /**
     * @brief Set the weight of the record
     *
     * Unless more than one QMdnsEngine::SRV record is being sent, this field
     * should be set to 0.
     */
    void setWeight(quint16 weight);

    /**
     * @brief Retrieve the port for the record
     *
     * This field is used by QMdnsEngine::SRV records.
     */
    quint16 port() const;

    /**
     * @brief Set the port for the record
     */
    void setPort(quint16 port);

    /**
     * @brief Retrieve attributes for the record
     *
     * This field is used by QMdnsEngine::TXT records.
     */
    QMap<QByteArray, QByteArray> attributes() const;

    /**
     * @brief Set attributes for the record
     */
    void setAttributes(const QMap<QByteArray, QByteArray> &attributes);

    /**
     * @brief Add an attribute to the record
     */
    void addAttribute(const QByteArray &key, const QByteArray &value);

    /**
     * @brief Retrieve the bitmap for the record
     *
     * This field is used by QMdnsEngine::NSEC records.
     */
    Bitmap bitmap() const;

    /**
     * @brief Set the bitmap for the record
     */
    void setBitmap(const Bitmap &bitmap);

private:

    RecordPrivate *const d;
};

QMDNSENGINE_EXPORT QDebug operator<<(QDebug dbg, const Record &record);

}

#endif // QMDNSENGINE_RECORD_H
