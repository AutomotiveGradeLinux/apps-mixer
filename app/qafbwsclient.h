#ifndef QAFBWSCLIENT_H
#define QAFBWSCLIENT_H

#include <QObject>
#include <QMap>
#include <QSharedPointer>
#include <QWebSocket>
#include <QJsonValue>

enum class AfMsgType
    : int
{
    Call = 2,
    RetOk = 3,
    RetErr = 4,
    Event = 5
};

class QAfbWsMsg;

class QAfbWsClient
    : public QObject
{
    Q_OBJECT
public:
    explicit QAfbWsClient(QObject* parent = nullptr);

    void open(const QUrl& url);
    void close();
    QUrl url() const;
    QSharedPointer<QAfbWsMsg> call(const QString &api, const QString &verb, const QJsonValue &args = QJsonValue());

signals:
    void connected();
    void disconnected();
    void error(QAbstractSocket::SocketError err, QString errStr);

private slots:
    void onSocketConnected();
    void onSocketDisconnected();
    void onSocketError(QAbstractSocket::SocketError err);
    void onSocketTextMessageReceived(QString msg);

private:
    int m_nextCallId;
    QWebSocket m_socket;
    QMap<int, QSharedPointer<QAfbWsMsg>> m_calls;
};

#endif // QAFBWSCLIENT_H
