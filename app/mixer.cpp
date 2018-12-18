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
#include "mixer.hpp"

Mixer::Mixer(QObject* parent)
	: QObject(parent)
{
	connect(&m_client, SIGNAL(connected()), this, SLOT(onClientConnected()));
	connect(&m_client, SIGNAL(disconnected()), this, SLOT(onClientDisconnected()));
	connect(&m_client, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onClientError(QAbstractSocket::SocketError)));
	connect(&m_client, SIGNAL(eventReceived(QString, const QJsonValue&)), this, SLOT(onClientEventReceived(QString, const QJsonValue&)));
}

void Mixer::open(const QUrl& url)
{
	m_url = url;
	TryOpen();
}

QList<QObject*> Mixer::roles() const
{
	return m_roles;
}

void Mixer::getRoleVolume(AudioRole* role)
{
	if (role == nullptr) return;

	QJsonObject arg;
	arg.insert("action", "volume");
	arg.insert("value", QJsonValue("+0")); // FIXME: Hack to get volume: ask for a relative change with a delta of zero

	m_client.call("ahl-4a", role->Name().toLocal8Bit().data(), arg, [role](bool r, const QJsonValue& v) {
		if (r && v.isObject())
		{
			qDebug() << role->Name() << " Volume changed: " << v;
			int newVolume = v.toObject()["response"].toObject()["volnew"].toInt();
			role->setValue(newVolume);
		}
	});
}

void Mixer::setRoleVolume(AudioRole* role)
{
	if (role == nullptr) return;
	role->BeginUpdate();

	QJsonObject arg;
	arg.insert("action", "volume");
	arg.insert("value", QJsonValue(role->Value()));
	m_client.call("ahl-4a", role->Name().toLocal8Bit().data(), arg, [role](bool r, const QJsonValue& v) {
		// Nothing to do, events will update sliders
		role->EndUpdate();
	});
}

void Mixer::onClientConnected()
{
	// Subscribe to 4a events
	m_client.call("ahl-4a", "subscribe", QJsonValue(), [this](bool r, const QJsonValue& val) {
		if (r) qDebug() << "Mixer::onClientConnected - subscribed to 4a events!";
		else qCritical () << "Mixer::onClientConnected - Failed to subscribe to 4a events!";
	});

	// Call HAL to populate list
	m_client.call("ahl-4a", "get_roles", QJsonValue(), [this](bool r, const QJsonValue& val) {
		if (r)
		{
			for(QObject* role : m_roles) delete role;
			m_roles.clear();

			QJsonArray cards = val.toObject()["response"].toArray();
			foreach (const QJsonValue& card, cards)
			{
				AudioRole* ar = new AudioRole(card.toString(), 0);
				getRoleVolume(reinterpret_cast<AudioRole*>(ar));
				connect(ar, SIGNAL(ValueChanged()), this, SLOT(onRoleValueChanged()));
				m_roles.append(ar);
				qDebug() << "Mixer::onClientConnected - added this HAL: " << card.toString();
			}
			emit rolesChanged();
		}
	});

}

void Mixer::onClientDisconnected()
{
	qDebug() << "Mixer::onClientDisconnected!";
	QTimer::singleShot(1000, this, SLOT(TryOpen()));
}

void Mixer::onClientError(QAbstractSocket::SocketError se)
{
	qDebug() << "Mixer::onClientError: " << se;
}

void Mixer::onClientEventReceived(QString eventName, const QJsonValue& data)
{
	qDebug() << "Mixer::onClientEventReceived[" << eventName << "]: " << data;
	if (eventName == "ahl-4a/volume_changed")
	{
		QString role = data.toObject()["role"].toString();
		int volume = data.toObject()["volume"].toInt();
		for(QObject* o : m_roles)
		{
			AudioRole* ar = reinterpret_cast<AudioRole*>(o);
			if (ar && ar->Name() == role)
			{
				ar->setValue(volume);
			}
		}
	}
}

void Mixer::onRoleValueChanged()
{
	AudioRole* role = reinterpret_cast<AudioRole*>(QObject::sender());
	if (role == nullptr) return;
	setRoleVolume(role);
}

void Mixer::TryOpen()
{
	qDebug() << "Mixer::TryOpen: " << m_url;
	m_client.open(m_url);
}
