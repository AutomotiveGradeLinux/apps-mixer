/*
 * Copyright (C) 2016 The Qt Company Ltd.
 * Copyright (C) 2016,2017 Konsulko Group
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
#pragma once

#include <QObject>
#include <QString>
#include <QSharedPointer>
#include <QStringList>
#include <QMap>
#include "qafbwebsocketclient.h"

class Mixer
    : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList roles READ roles NOTIFY rolesChanged)

private:
    QUrl m_url;
    QMap<QString, int> m_volumes;
    QStringList m_roles;
    QAfbWebsocketClient m_client;

public:
    explicit Mixer(QObject* parent = nullptr);
    Mixer(const Mixer&) = delete;

    Q_INVOKABLE void open(const QUrl& url);
    Q_INVOKABLE QStringList roles() const;
    Q_INVOKABLE void setVolume(const QString& name, int value);
    Q_INVOKABLE void getVolume(const QString& name);

signals:
    void rolesChanged();
    void volumeChanged(const QString& name, int value);

private slots:
    void onClientConnected();
    void onClientDisconnected();
    void onClientError(QAbstractSocket::SocketError se);
    void onRetryOpen();
    void onClientEventReceived(QString eventName, const QJsonValue& data);
};
