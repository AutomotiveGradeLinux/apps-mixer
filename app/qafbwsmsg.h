/*
 * Copyright (C) 2018 IoT.bzh
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef QAFBWSMSG_H
#define QAFBWSMSG_H

#include <QObject>
#include <QJsonValue>
#include "qafbwsclient.h"

class QAfbWsMsg
    : public QObject
{
    Q_OBJECT
public:
    explicit QAfbWsMsg(int callId = 0, const QString& api = "", const QString& verb = "", QObject* parent = nullptr);
    void close(AfMsgType type, const QJsonValue& result);

    int callId() const;
    AfMsgType messageType() const;
    QString api() const;
    QString verb() const;
    QJsonValue value() const;

signals:
    void closed();

private:
    int m_callId;
    AfMsgType m_type;
    QString m_api;
    QString m_verb;
    QJsonValue m_value;
};

#endif // QAFBWSMSG_H
