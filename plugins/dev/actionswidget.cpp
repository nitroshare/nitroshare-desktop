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

#include <QFrame>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QMessageBox>
#include <QVBoxLayout>

#include <nitroshare/action.h>
#include <nitroshare/actionregistry.h>
#include <nitroshare/application.h>

#include "actionswidget.h"

ActionsWidget::ActionsWidget(Application *application)
    : mApplication(application),
      mActions(new QComboBox),
      mDescription(new QLabel),
      mParams(new QTextEdit("{\n    \n}")),
      mInvoke(new QPushButton(tr("Invoke"))),
      mReturn(new QTextEdit)
{
    QLabel *instructionsLabel = new QLabel(tr(
        "Use this tool to invoke an action. "
        "Parameters are supplied as a JSON object."
    ));
    instructionsLabel->setWordWrap(true);

    QFrame *frame = new QFrame;
    frame->setFrameShape(QFrame::HLine);
    frame->setFrameShadow(QFrame::Sunken);

    // Add the existing actions
    foreach (Action *action, mApplication->actionRegistry()->actions()) {
        mActions->addItem(action->name(), QVariant::fromValue(action));
    }

    // Monitor for new actions being added / removed
    connect(mApplication->actionRegistry(), &ActionRegistry::actionAdded, this, &ActionsWidget::onActionAdded);
    connect(mApplication->actionRegistry(), &ActionRegistry::actionRemoved, this, &ActionsWidget::onActionRemoved);

    // Make the text for the label stand out
    mDescription->setEnabled(false);

    // Only plain text is supported
    mParams->setAcceptRichText(false);

    connect(mInvoke, &QPushButton::clicked, this, &ActionsWidget::onInvoke);

    mReturn->setReadOnly(true);

    QVBoxLayout *vboxLayout = new QVBoxLayout;
    vboxLayout->addWidget(instructionsLabel);
    vboxLayout->addWidget(frame);
    vboxLayout->addWidget(new QLabel(tr("Action:")));
    vboxLayout->addWidget(mActions);
    vboxLayout->addWidget(new QLabel(tr("Description:")));
    vboxLayout->addWidget(mDescription);
    vboxLayout->addWidget(new QLabel(tr("Parameters:")));
    vboxLayout->addWidget(mParams);
    vboxLayout->addWidget(mInvoke);
    vboxLayout->addWidget(new QLabel(tr("Return value:")));
    vboxLayout->addWidget(mReturn);
    setLayout(vboxLayout);
}

void ActionsWidget::onActionAdded(Action *action)
{
    mActions->addItem(action->name(), QVariant::fromValue(action));
}

void ActionsWidget::onActionRemoved(Action *action)
{
    mActions->removeItem(mActions->findText(action->name()));
}

void ActionsWidget::onInvoke()
{
    // Parse the parameters as JSON
    QJsonParseError jsonError;
    QJsonDocument document = QJsonDocument::fromJson(mParams->toPlainText().toUtf8(), &jsonError);
    if (jsonError.error != QJsonParseError::NoError) {
        mReturn->setPlainText(QString("error: %1").arg(jsonError.errorString()));
        return;
    }

    // Convert the JSON object into a variant map and invoke the action
    QVariantMap params = document.object().toVariantMap();
    QVariant ret = mActions->currentData().value<Action*>()->invoke(params);

    // Attempt to convert the return value to a string if possible
    if (ret.canConvert(QVariant::String)) {
        ret.convert(QVariant::String);
    }

    // Display the return value in the best way possible
    switch (ret.type()) {
    case QVariant::String:
        mReturn->setPlainText(ret.toString());
        break;
    case QVariant::List:
        mReturn->setPlainText(
            QJsonDocument(QJsonArray::fromVariantList(ret.toList())).toJson()
        );
        break;
    case QVariant::Map:
        mReturn->setPlainText(
            QJsonDocument(QJsonObject::fromVariantMap(ret.toMap())).toJson()
        );
        break;
    default:
        break;
    }
}
