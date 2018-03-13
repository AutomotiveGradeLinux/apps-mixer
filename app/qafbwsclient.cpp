#include "qafbwsclient.h"
#include "qafbwsmsg.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QtDebug>

QAfbWsClient::QAfbWsClient(QObject* parent)
    : QObject{parent}
    , m_nextCallId{0}
{
    connect(&m_socket, SIGNAL(connected()), this, SLOT(onSocketConnected()));
    connect(&m_socket, SIGNAL(disconnected()), this, SLOT(onSocketDisconnected()));
    connect(&m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onSocketError(QAbstractSocket::SocketError)));
}

QUrl QAfbWsClient::url() const
{
    return m_socket.requestUrl();
}

QSharedPointer<QAfbWsMsg> QAfbWsClient::call(const QString& api, const QString& verb, const QJsonValue& args)
{
    while(m_calls.constFind(m_nextCallId) != m_calls.cend()) m_nextCallId++; // Make sure that callId is not currently used

    qDebug() << "QAfbWsClient::call(" << api << ", " << verb << ", " << args << ")";
    QJsonArray arr;
    arr.append(static_cast<int>(AfMsgType::Call));
    arr.append(m_nextCallId);
    arr.append(api + "/" + verb);
    arr.append(args);

    QJsonDocument doc;
    doc.setArray(arr);

    QSharedPointer<QAfbWsMsg> msg(new QAfbWsMsg(m_nextCallId, api, verb));
    m_calls[m_nextCallId] = msg;

    // TODO: handle failure of sendTextMessage
    m_socket.sendTextMessage(doc.toJson(QJsonDocument::Compact));
    qDebug() << "m_socket.sendTextMessage(" << doc.toJson(QJsonDocument::Compact) << ")";

    m_nextCallId++;

    return msg;
}

void QAfbWsClient::onSocketConnected()
{
    qDebug() << "QAfbWsClient::onSocketConnected()";
    connect(&m_socket, SIGNAL(textMessageReceived(QString)), this, SLOT(onSocketTextMessageReceived(QString)));
    emit connected();
}

void QAfbWsClient::onSocketDisconnected()
{
    qDebug() << "QAfbWsClient::onSocketDisconnected()";
    m_nextCallId = 0;
    disconnect(&m_socket, SIGNAL(textMessageReceived(QString)), this, SLOT(onSocketTextMessageReceived(QString)));
    emit disconnected();
}

void QAfbWsClient::onSocketError(QAbstractSocket::SocketError err)
{
    qDebug() << "QAfbWsClient::onSocketError(" << err << ")";
    emit error(err, m_socket.errorString());
}

void QAfbWsClient::onSocketTextMessageReceived(QString msg)
{
    qDebug() << "QAfbWsClient::onSocketTextMessageReceived(" << msg << ")";

    if (msg.size() == 0)
    {
        qCritical() << "QAfbWsClient::onSocketTextMessageReceived: received an empty message.";
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(msg.toUtf8());
    if (doc.isEmpty() || doc.isNull() || !doc.isArray())
    {
        qCritical() << "QAfbWsClient::onSocketTextMessageReceived: received an invalid message.";
        return;
    }

    QJsonArray arr = doc.array();
    AfMsgType msgType = static_cast<AfMsgType>(arr[0].toInt());
    int callId = arr[1].isString() ? arr[1].toString().toInt() : arr[1].toInt();
    QString api;
    QJsonValue value;

    switch(msgType)
    {
    case AfMsgType::Call:
        qCritical() << "QAfbWsClient::onSocketTextMessageReceived: Client received a call, which should not happen.";
        break;

    case AfMsgType::Event:
        qDebug() << "QAfbWsClient::onSocketTextMessageReceived: Client received an event.";
        // TODO: handle events
        value = arr[3];
        break;

    case AfMsgType::RetOk:
    case AfMsgType::RetErr:
        {
            value = arr[2];
            QMap<int, QSharedPointer<QAfbWsMsg>>::const_iterator it = m_calls.find(callId);
            if (it == m_calls.end())
            {
                qCritical() << "QAfbWsClient::onSocketTextMessageReceived: received a response to a not found query.";
                return;
            }

            QSharedPointer<QAfbWsMsg> m = *it;
            m_calls.remove(callId);
            m->close(msgType, value);
        }
        break;

    default:
        qCritical() << "QAfbWsClient::onSocketTextMessageReceived: Client received an unsupported message.";
        break;
    }
}

void QAfbWsClient::open(const QUrl& url)
{
    qDebug() << "QAfbWsClient::open(" << url << ")";
    m_socket.open(url);
}

void QAfbWsClient::close()
{
    qDebug() << "QAfbWsClient::close()";
    m_socket.close();
}
