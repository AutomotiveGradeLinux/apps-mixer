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

#include <QtCore/QHash>
#include <QtCore/QObject>

const char * const channel_position_string[] =
{
	"Mono",
	"Front Left",
	"Front Right",
	"Center",
	"Rear Center",
	"Rear Left",
	"Rear Right",
	"LFE",
	"Left Center",
	"Right Center",
	"Side Left",
	"Side Right",
};

enum control_type
{
	C_SOURCE,
	C_SINK
};

typedef struct
{
	uint32_t type;
	uint32_t index;
	pa_cvolume cvolume;
} CState;

class PaClient : public QObject
{
	Q_OBJECT
	public:
		PaClient();
		~PaClient();

		void init();
		void close();

 		inline pa_context *context() const
		{
			return m_ctx;
		}

		inline void lock()
		{
			pa_threaded_mainloop_lock(m_ml);
		}

		inline void unlock()
		{
			pa_threaded_mainloop_unlock(m_ml);
		}

		void addOneControlState(int type, int index, const pa_cvolume *cvolume);

		QHash<int, pa_cvolume *> sink_states();
		QHash<int, pa_cvolume *> source_states();

	public slots:
		void setVolume(uint32_t type, uint32_t index, uint32_t channel, uint32_t volume);

	signals:
		void controlAdded(int cindex, QString name, QString desc, int type, int channel, const char *cdesc, int volume);
		void volumeExternallyChanged(uint32_t type, uint32_t cindex, uint32_t channel, uint32_t volume);

	private:
		bool m_init;
		pa_threaded_mainloop *m_ml;
		pa_mainloop_api *m_mlapi;
		pa_context *m_ctx;
		QHash<int, pa_cvolume *> m_sink_states;
		QHash<int, pa_cvolume *> m_source_states;

	public slots:
};
