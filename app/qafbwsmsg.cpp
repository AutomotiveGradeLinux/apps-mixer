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
