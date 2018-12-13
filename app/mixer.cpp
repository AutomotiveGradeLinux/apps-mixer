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

#include <QJsonArray>
#include <QJsonObject>
#include <QTimer>
#include <QtDebug>
#include "mixer.h"

Mixer::Mixer(QObject* parent)
    : QObject(parent)
{
	connect(&m_client, SIGNAL(connected()), this, SLOT(onClientConnected()));
	connect(&m_client, SIGNAL(disconnected()), this, SLOT(onClientDisconnected()));
	connect(&m_client, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onClientError(QAbstractSocket::SocketError)));
	connect(&m_client, SIGNAL(eventReceived(QString, const QJsonValue&)), this, SLOT(onClientEventReceived(QString, const QJsonValue&)));

	m_roles.append("Multimedia");
	m_roles.append("Navigation");
	m_roles.append("Emergency");
}

QStringList Mixer::roles() const
{
	return m_roles;
}

void Mixer::open(const QUrl& url)
{
	m_url = url;
	m_client.open(m_url);
}

void Mixer::onClientConnected()
{
	// Subscribe to 4a events
	m_client.call("ahl-4a", "subscribe", QJsonValue(), [this](bool r, const QJsonValue& val) {
		if (r)
			qDebug() << "Mixer::onClientConnected - subscribed to 4a events!";
		else
			qCritical () << "Mixer::onClientConnected - Failed to subscribe to 4a events!";
	});

	// Call HAL to populate list
	m_client.call("ahl-4a", "get_roles", QJsonValue(), [this](bool r, const QJsonValue& val) {
	        if (r)
	        {
			m_roles.clear();
			//BUG: should be able to add this, but not handled right now: m_roles.append("playback");
			QJsonArray cards = val.toObject()["response"].toArray();
			foreach (const QJsonValue& card, cards)
			{
				m_roles.append(card.toString());
				qDebug() << "Mixer::onClientConnected - added this HAL: " << card.toString();
			}
			emit rolesChanged();
		}
	});
}

void Mixer::onClientDisconnected()
{
	qDebug() << "Mixer::onClientDisconnected";
	QTimer::singleShot(1000, this, SLOT(onRetryOpen()));
}

void Mixer::onClientError(QAbstractSocket::SocketError se)
{
	qDebug() << "Mixer::onClientError: " << se;
}

void Mixer::onRetryOpen()
{
	m_client.open(m_url);
}

void Mixer::onClientEventReceived(QString eventName, const QJsonValue& data)
{
	qDebug() << "Mixer::onClientEventReceived[" << eventName << "]: " << data;
	if (eventName == "ahl-4a/volume_changed")
	{
		QString role = data["role"].toString();
		int volume = data["volume"].toInt();
		m_volumes[role] = volume;
		emit volumeChanged(role, volume);
	}
}

void Mixer::setVolume(const QString& name, int value)
{
	auto currentVolume = m_volumes.find(name);
	if (currentVolume != m_volumes.end() && *currentVolume == value)
        return;

	QJsonObject arg;
	arg.insert("action", "volume");
	arg.insert("value", QJsonValue(value));
	m_client.call("ahl-4a", name, arg, [name](bool r, const QJsonValue& v) {
		/* Nothing to do, events will update sliders*/
	});
}

void Mixer::getVolume(const QString& name)
{
	QJsonObject arg;
	arg.insert("action", "volume");
	arg.insert("value", QJsonValue("+0")); // FIXME: Hack to get volume: ask for a relative change with a delta of zero
	m_client.call("ahl-4a", name, arg, [this, name](bool r, const QJsonValue& v) {
		if (r && v.isObject())
		{
			// TODO: Success, update the slider
			qDebug() << "Volume changed: " << v;
			int newVolume = v.toObject()["response"].toObject()["volnew"].toInt();
			auto currentVolume = m_volumes.find(name);
			if (currentVolume != m_volumes.end() && *currentVolume == newVolume)
				return;

			m_volumes[name] = newVolume;
			emit volumeChanged(name, newVolume);
		}
	});
}
