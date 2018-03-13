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
