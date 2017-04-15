/*
 * Copyright (C) 2016,2017 Konsulko Group
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

#include <pulse/pulseaudio.h>

#include <QtCore/QAbstractListModel>
#include <QtCore/QList>

class PaControlModel;

class PaControl
{
	public:
		PaControl(const quint32 &index, const QString &desc, const quint32 &type, const quint32 &channel, const QString &cdesc, const quint32 &volume);

		quint32 cindex() const;
		QString desc() const;
		quint32 type() const;
		quint32 channel() const;
		QString cdesc() const;
		quint32 volume() const;
		void setCIndex(const QVariant&);
		void setDesc(const QVariant&);
		void setType(const QVariant&);
		void setChannel(const QVariant&);
		void setCDesc(const QVariant&);
		void setVolume(PaControlModel *, const QVariant&);

	private:
		quint32 m_cindex;
		QString m_desc;
		quint32 m_type;
		quint32 m_channel;
		QString m_cdesc;
		quint32 m_volume;
};

class PaControlModel : public QAbstractListModel
{
	Q_OBJECT
	public:
		enum PaControlRoles {
			CIndexRole = Qt::UserRole + 1,
			DescRole,
			TypeRole,
			ChannelRole,
			CDescRole,
			VolumeRole
		};

		PaControlModel(QObject *parent = 0);

		void addControl(const PaControl &control);

		int rowCount(const QModelIndex &parent = QModelIndex()) const;

		QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

		bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

		Qt::ItemFlags flags(const QModelIndex &index) const;

	public slots:
		void addOneControl(int cindex, QString desc, int type, int channel, const char *cdesc, int volume);

	signals:
		void volumeChanged(uint32_t type, uint32_t index, uint32_t channel, uint32_t volume);

	protected:
		QHash<int, QByteArray> roleNames() const;
	private:
		QList<PaControl> m_controls;
		pa_context *pa_ctx;
};
