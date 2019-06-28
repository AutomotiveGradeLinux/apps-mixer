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

void Mixer::setRoleVolume(AudioRole* role)
{
	if (role == nullptr) return;

	QJsonObject arg;
	arg.insert("control", role->Name().toLocal8Bit().data());
	arg.insert("value", QJsonValue(role->Value() / 100.0));

	m_client.call("audiomixer", "volume", arg);
}

void Mixer::parseControls(const QJsonValue & v)
{
	qDebug() << "got controls: " << v;

	for(QObject* role : m_roles) delete role;
	m_roles.clear();

	for (const QJsonValue & av : v.toArray()) {
		QString name = av.toObject()["control"].toString();
		int value = av.toObject()["volume"].toDouble() * 100;
		value = qBound(0, value, 100);

		AudioRole *ar = new AudioRole(name, value);
		connect(ar, SIGNAL(ValueChanged()), this, SLOT(onRoleValueChanged()));
		m_roles.append(ar);

		qDebug() << "added role: " << ar->Name()
			<< " value: " << ar->Value();
	}

	emit rolesChanged();
}

void Mixer::onClientConnected()
{
	m_client.call("audiomixer", "list_controls", QJsonObject(), [this](bool r, const QJsonValue& v) {
		if (r && v.isObject()) {
			parseControls(v.toObject()["response"]);
		}

		QJsonObject arg;
		arg.insert("event", "controls_changed");
		m_client.call("audiomixer", "subscribe", arg);

		arg.insert("event", "volume_changed");
		m_client.call("audiomixer", "subscribe", arg);
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

	if (eventName == "audiomixer/controls_changed") {
		m_client.call("audiomixer", "list_controls", QJsonObject(), [this](bool r, const QJsonValue& v) {
			if (r && v.isObject()) {
				parseControls(v.toObject()["response"]);
			}
		});
	}
	else if (eventName == "audiomixer/volume_changed") {
		QString name = data.toObject()["control"].toString();
		int value = data.toObject()["value"].toDouble() * 100;
		value = qBound(0, value, 100);

		for (QObject *o : m_roles) {
			AudioRole *ar = static_cast<AudioRole*>(o);
			if (ar->Name() == name) {
				ar->BeginUpdate();
				ar->setValue(value);
				break;
			}
		}
	}
}

void Mixer::onRoleValueChanged()
{
	AudioRole* role = qobject_cast<AudioRole*>(QObject::sender());
	if (role == nullptr) return;

	/* if the role was not being updated by us, it was modified from the UI,
	   in which case we have to set it to the backend */
	if (!role->EndUpdate())
		setRoleVolume(role);
}

void Mixer::TryOpen()
{
	qDebug() << "Mixer::TryOpen: " << m_url;
	m_client.open(m_url);
}
