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

#include "qafbwsmsg.h"

QAfbWsMsg::QAfbWsMsg(int callId, const QString& api, const QString& verb, QObject* parent)
    : QObject{parent}
    , m_callId{callId}
{
    m_api = api;
    m_verb = verb;
}

void QAfbWsMsg::close(AfMsgType type, const QJsonValue& result)
{
    m_type = type;
    m_value = result;

    qDebug() << "QAfbWsMsg::close: type=" << static_cast<int>(m_type) << ", api=" << m_api << ", verb=" << m_verb << ", value=" << m_value;

    emit closed();
}

int QAfbWsMsg::callId() const
{
    return m_callId;
}

AfMsgType QAfbWsMsg::messageType() const
{
    return m_type;
}

QString QAfbWsMsg::api() const
{
    return m_api;
}

QString QAfbWsMsg::verb() const
{
    return m_verb;
}

QJsonValue QAfbWsMsg::value() const
{
    return m_value;
}
