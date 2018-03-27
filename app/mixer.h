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

#ifndef MIXER_H
#define MIXER_H

#include <QObject>
#include <QSharedPointer>
#include <QVariantList>
#include "qafbwsclient.h"
#include "qafbwsmsg.h"

class Mixer
    : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList hals READ hals NOTIFY halsChanged)
    Q_PROPERTY(QString activeHal READ activeHal WRITE setActiveHal NOTIFY activeHalChanged)

    Q_PROPERTY(int masterVolume READ masterVolume WRITE setMasterVolume NOTIFY masterVolumeChanged)
    Q_PROPERTY(int pcmVolume READ pcmVolume WRITE setPcmVolume NOTIFY pcmVolumeChanged)
    Q_PROPERTY(int microphoneVolume READ microphoneVolume WRITE setMicrophoneVolume NOTIFY microphoneVolumeChanged)

public:
    explicit Mixer(QObject* parent = nullptr);
    Mixer(const Mixer&) = delete;

    Q_INVOKABLE void open(const QUrl& url);
    Q_INVOKABLE QVariantList hals() const;
    Q_INVOKABLE QString activeHal() const;
    Q_INVOKABLE void setActiveHal(QString h);

    Q_INVOKABLE int masterVolume() const;
    Q_INVOKABLE int pcmVolume() const;
    Q_INVOKABLE int microphoneVolume() const;

    Q_INVOKABLE void setMasterVolume(int v);
    Q_INVOKABLE void setPcmVolume(int v);
    Q_INVOKABLE void setMicrophoneVolume(int v);

signals:
    void halsChanged();
    void activeHalChanged();
    void masterVolumeChanged();
    void pcmVolumeChanged();
    void microphoneVolumeChanged();

private slots:
    void onClientConnected();
    void onHalListClosed();
    void onGetMasterVolume();
    void onGetPcmVolume();
    void onGetMicrophoneVolume();
    void onSetMasterVolume();
    void onSetPcmVolume();
    void onSetMicrophoneVolume();

private:
    int m_masterVolume;
    int m_pcmVolume;
    int m_microphoneVolume;
    int m_masterVolumePending;
    int m_pcmVolumePending;
    int m_microphoneVolumePending;
    QAfbWsClient m_client;
    QVariantList m_hallist;
    QString m_activeHal;
    QSharedPointer<QAfbWsMsg> m_alsacoreHallist;
    QSharedPointer<QAfbWsMsg> m_getMasterVolume;
    QSharedPointer<QAfbWsMsg> m_getPcmVolume;
    QSharedPointer<QAfbWsMsg> m_getMicrophoneVolume;
    QSharedPointer<QAfbWsMsg> m_setMasterVolume;
    QSharedPointer<QAfbWsMsg> m_setPcmVolume;
    QSharedPointer<QAfbWsMsg> m_setMicrophoneVolume;
};

#endif // MIXER_H
