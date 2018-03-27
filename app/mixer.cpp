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
    , m_masterVolume{0}
    , m_pcmVolume{0}
    , m_microphoneVolume{0}
{
    connect(&m_client, SIGNAL(connected()), this, SLOT(onClientConnected()));
}

QVariantList Mixer::hals() const
{
    return m_hallist;
}

int Mixer::masterVolume() const
{
    return m_masterVolume;
}

int Mixer::pcmVolume() const
{
    return m_pcmVolume;
}

int Mixer::microphoneVolume() const
{
    return m_microphoneVolume;
}

void Mixer::setMasterVolume(int v)
{
    qDebug() << "Mixer::setMasterVolume(" << v << ")";
    if (v != m_masterVolume)
    {
        m_masterVolumePending = v;
        QJsonObject arg;
        arg["label"] = "Master_Playback_Volume";
        arg["val"] = static_cast<int>(v);
        m_setMasterVolume = m_client.call(m_activeHal, "ctlset", arg);
        connect(m_setMasterVolume.data(), SIGNAL(closed()), this, SLOT(onSetMasterVolume()));
    }
}

void Mixer::setPcmVolume(int v)
{
    qDebug() << "Mixer::setPcmVolume(" << v << ")";
    if (v != m_pcmVolume)
    {
        m_pcmVolumePending = v;
        QJsonObject arg;
        arg["label"] = "PCM_Playback_Volume";
        arg["val"] = static_cast<int>(v);
        m_setPcmVolume = m_client.call(m_activeHal, "ctlset", arg);
        connect(m_setPcmVolume.data(), SIGNAL(closed()), this, SLOT(onSetPcmVolume()));
    }
}

void Mixer::setMicrophoneVolume(int v)
{
    qDebug() << "Mixer::setMicrophoneVolume(" << v << ")";
    if (v != m_microphoneVolume)
    {
        m_microphoneVolumePending = v;
        QJsonObject arg;
        arg["label"] = "Capture_Volume";
        arg["val"] = static_cast<int>(v);
        m_setMicrophoneVolume = m_client.call(m_activeHal, "ctlset", arg);
        connect(m_setMicrophoneVolume.data(), SIGNAL(closed()), this, SLOT(onSetMicrophoneVolume()));
    }
}

QString Mixer::activeHal() const
{
    return m_activeHal;
}

void Mixer::setActiveHal(QString h)
{
    if (h != m_activeHal)
    {
        m_activeHal = h;
        qDebug() << "Mixer::setActiveHal: " << h;
        // Get volumes for this card

        QJsonObject arg;

        arg["label"] = "Master_Playback_Volume";
        m_getMasterVolume = m_client.call(m_activeHal, "ctlget", arg);
        connect(m_getMasterVolume.data(), SIGNAL(closed()), this, SLOT(onGetMasterVolume()));

        arg["label"] = "PCM_Playback_Volume";
        m_getPcmVolume = m_client.call(m_activeHal, "ctlget", arg);
        connect(m_getPcmVolume.data(), SIGNAL(closed()), this, SLOT(onGetPcmVolume()));

        arg["label"] = "Capture_Volume";
        m_getMicrophoneVolume = m_client.call(m_activeHal, "ctlget", arg);
        connect(m_getMicrophoneVolume.data(), SIGNAL(closed()), this, SLOT(onGetMicrophoneVolume()));

        emit activeHalChanged();
    }
}

void Mixer::open(const QUrl &url)
{
    m_client.open(url);
}

void Mixer::onClientConnected()
{
    // Call HAL to populate list
    m_alsacoreHallist = m_client.call("alsacore", "hallist");
    connect(m_alsacoreHallist.data(), SIGNAL(closed()), this, SLOT(onHalListClosed()));
}

void Mixer::onHalListClosed()
{
    qDebug() << "Mixer::onHalListClosed";
    if(m_alsacoreHallist->messageType() == AfMsgType::RetOk)
    {
        m_hallist.clear();

        QJsonArray cards = m_alsacoreHallist->value().toObject()["response"].toArray();
        qDebug() << "Mixer::onHalListClosed - founds: " << cards;
        foreach (const QJsonValue& card, cards)
        {
            QJsonObject c = card.toObject();
            QVariant v(c["api"].toString());
            m_hallist.append(v);
            qDebug() << "Mixer::onHalListClosed - added this HAL: " << v;
        }

        m_alsacoreHallist.clear();

        setActiveHal(m_hallist[0].toString());

        emit halsChanged();
    }
}

void Mixer::onGetMasterVolume()
{
    qDebug() << "Mixer::onGetMasterVolume()";
    disconnect(m_getMasterVolume.data(), SIGNAL(closed()), this, SLOT(onGetMasterVolume()));
    if (m_getMasterVolume->messageType() == AfMsgType::RetOk && m_getMasterVolume->value().isObject())
    {
        setMasterVolume(m_getMasterVolume->value().toObject()["response"].toObject()["val"].toArray()[0].toInt());
    }
}

void Mixer::onGetPcmVolume()
{
    qDebug() << "Mixer::onGetPcmVolume()";
    disconnect(m_getPcmVolume.data(), SIGNAL(closed()), this, SLOT(onGetPcmVolume()));
    if (m_getPcmVolume->messageType() == AfMsgType::RetOk && m_getPcmVolume->value().isObject())
    {
        setPcmVolume(m_getPcmVolume->value().toObject()["response"].toObject()["val"].toArray()[0].toInt());
    }
}

void Mixer::onGetMicrophoneVolume()
{
    qDebug() << "Mixer::onGetMicrophoneVolume()";
    disconnect(m_getMicrophoneVolume.data(), SIGNAL(closed()), this, SLOT(onGetMicrophoneVolume()));
    if (m_getMicrophoneVolume->messageType() == AfMsgType::RetOk && m_getMicrophoneVolume->value().isObject())
    {
        setMicrophoneVolume(m_getMicrophoneVolume->value().toObject()["response"].toObject()["val"].toArray()[0].toInt());
    }
}

void Mixer::onSetMasterVolume()
{
    qDebug() << "Mixer::onSetMasterVolume()";
    disconnect(m_setMasterVolume.data(), SIGNAL(closed()), this, SLOT(onSetMasterVolume()));
    if (m_setMasterVolume->messageType() == AfMsgType::RetOk && m_setMasterVolume->value().isObject())
    {
        m_masterVolume = m_masterVolumePending;
        emit masterVolumeChanged();
    }
}

void Mixer::onSetPcmVolume()
{
    qDebug() << "Mixer::onSetPcmVolume()";
    disconnect(m_setPcmVolume.data(), SIGNAL(closed()), this, SLOT(onSetPcmVolume()));
    if (m_setPcmVolume->messageType() == AfMsgType::RetOk && m_setPcmVolume->value().isObject())
    {
        m_pcmVolume = m_pcmVolumePending;
        emit pcmVolumeChanged();
    }
}

void Mixer::onSetMicrophoneVolume()
{
    qDebug() << "Mixer::onSetMicrophoneVolume()";
    disconnect(m_setMicrophoneVolume.data(), SIGNAL(closed()), this, SLOT(onSetMicrophoneVolume()));
    if (m_setMicrophoneVolume->messageType() == AfMsgType::RetOk && m_setMicrophoneVolume->value().isObject())
    {
        m_microphoneVolume = m_microphoneVolumePending;
        qDebug() << "Mixer::onSetMicrophoneVolume() - ";
        emit microphoneVolumeChanged();
    }
}


