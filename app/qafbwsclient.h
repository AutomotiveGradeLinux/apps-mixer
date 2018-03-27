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
