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
#include <QtDebug>
#include "mixer.h"

Mixer::Mixer(QObject* parent)
    : QObject(parent)
{
    connect(&m_client, SIGNAL(connected()), this, SLOT(onClientConnected()));
}

QStringList Mixer::roles() const
{
    return m_roles;
}

void Mixer::open(const QUrl &url)
{
    m_client.open(url);
}

void Mixer::onClientConnected()
{
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

void Mixer::setVolume(const QString& name, int value)
{
    QJsonObject arg;
    arg.insert("action", "volume");
    arg.insert("value", QJsonValue(value));
    m_client.call("ahl-4a", name, arg, [name](bool r, const QJsonValue& v) {
        if (r && v.isObject())
        {
			// TODO: Success, update the slider
        }
        else
        {
			// TODO: Failed, reset the slider to previous value
        }
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
            int newVolume = v.toObject()["response"].toObject()["volnew"].toInt();
            auto currentVolume = volumes_.find(name);
            if (currentVolume != volumes_.end() && *currentVolume == newVolume)
                return;

            volumes_[name] = newVolume;
            emit volumeChanged(name, newVolume);
        }
        else
        {
			// TODO: Failed, what to do ?
        }
    });
}
