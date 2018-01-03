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

#include <iostream>

#include <QCoreApplication>
#include <QObject>
#include <QVariantMap>

#include <nitroshare/apiutil.h>

/**
 * @brief Print usage information
 */
void printUsage()
{
    std::cerr << "NitroShare CLI client\n"
              << "Version: " << NITROSHARE_VERSION << "\n\n"
              << "Usage: " << QCoreApplication::arguments().at(0).toUtf8().constData()
              << " ACTION [PARAMS]\n\n";
}

/**
 * @brief Parse the command line arguments
 * @param action storage for the action
 * @param params storage for the action parameters
 * @param error description of error
 * @return true if the arguments were parsed
 */
bool parseArguments(QString &action, QVariantMap &params, QString &error)
{
    QStringList args = QCoreApplication::arguments();

    // Ensure at least two parameters were supplied
    if (args.count() < 2) {
        error = QObject::tr("action missing");
        return false;
    }

    action = args.at(1);

    // Parse the parameters
    for (auto i = args.constBegin() + 2; i != args.constEnd(); ++i) {
        if (!(*i).startsWith("--")) {
            error = QObject::tr("parameter must start with \"--\"");
            return false;
        }
        QString paramName = *(i++);
        if (i == args.constEnd()) {
            error = QObject::tr("missing value for \"%1\"").arg(paramName);
            return false;
        }
        params.insert(paramName, *i);
    }

    return true;
}

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    Q_UNUSED(app)

    QString action;
    QVariantMap params;
    QString error;

    // Parse the arguments
    if (!parseArguments(action, params, error)) {
        printUsage();
        std::cerr << "Error: " << error.toUtf8().constData() << std::endl;
        return 1;
    }

    // Send the request
    QVariant returnVal;
    if (!ApiUtil::sendRequest(action, params, returnVal, &error)) {
        std::cerr << "Error: " << error.toUtf8().constData() << std::endl;
        return 1;
    }

    return 0;
}
