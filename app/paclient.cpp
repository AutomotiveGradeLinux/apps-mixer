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

#include "paclient.h"

#include <QtCore/QDebug>

PaClient::PaClient()
	: m_init(false), m_ml(nullptr), m_mlapi(nullptr), m_ctx(nullptr)
{
}

PaClient::~PaClient()
{
	if (m_init)
		close();
}

void PaClient::close()
{
	if (!m_init) return;
	pa_threaded_mainloop_stop(m_ml);
	pa_threaded_mainloop_free(m_ml);
	m_init = false;
}

static void set_sink_volume_cb(pa_context *c, int success, void *data)
{
	Q_UNUSED(data);

	if (!success)
		qWarning() << "PaClient: set sink volume: " <<
			pa_strerror(pa_context_errno(c));
}

static void set_source_volume_cb(pa_context *c, int success, void *data)
{
	Q_UNUSED(data);

	if (!success)
		qWarning() << "PaClient: set source volume: " <<
			pa_strerror(pa_context_errno(c));
}

void PaClient::setVolume(uint32_t type, uint32_t index, uint32_t channel, uint32_t volume)
{
	pa_operation *o;
	pa_context *c = context();
	pa_cvolume *cvolume = NULL;

	if (type == C_SINK) {
		cvolume = m_sink_states.value(index);
		cvolume->values[channel] = volume;
		if (!(o = pa_context_set_sink_volume_by_index(c, index, cvolume, set_sink_volume_cb, NULL))) {
			qWarning() << "PaClient: set sink #" << index <<
				" channel #" << channel <<
				" volume: " << pa_strerror(pa_context_errno(c));
			return;
		}
		pa_operation_unref(o);
	} else if (type == C_SOURCE) {
		cvolume = m_source_states.value(index);
		cvolume->values[channel] = volume;
		if (!(o = pa_context_set_source_volume_by_index(c, index, cvolume, set_source_volume_cb, NULL))) {
			qWarning() << "PaClient: set source #" << index <<
				" channel #" << channel <<
				" volume: " << pa_strerror(pa_context_errno(c));
			return;
		}
		pa_operation_unref(o);
	}
}

void get_source_list_cb(pa_context *c,
		const pa_source_info *i,
		int eol,
		void *data)
{
	int chan;

	PaClient *self = reinterpret_cast<PaClient*>(data);

	if (eol < 0) {
		qWarning() << "PaClient: get source list: " <<
			pa_strerror(pa_context_errno(c));

		self->close();
		return;
	}

	if (!eol) {
		self->addOneControlState(C_SOURCE, i->index, &i->volume);
		for (chan = 0; chan < i->channel_map.channels; chan++) {
			emit self->controlAdded(i->index, QString(i->description), C_SOURCE, chan,
						channel_position_string[i->channel_map.map[chan]],
						i->volume.values[chan]);
		}
	}
}

void get_sink_list_cb(pa_context *c,
		const pa_sink_info *i,
		int eol,
		void *data)
{
	PaClient *self = reinterpret_cast<PaClient*>(data);
	int chan;

	if(eol < 0) {
		qWarning() << "PaClient: get sink list: " <<
			pa_strerror(pa_context_errno(c));
		self->close();
		return;
	}

	if(!eol) {
		self->addOneControlState(C_SINK, i->index, &i->volume);
		for (chan = 0; chan < i->channel_map.channels; chan++) {
			emit self->controlAdded(i->index, QString(i->description), C_SINK, chan,
						 channel_position_string[i->channel_map.map[chan]],
						 i->volume.values[chan]);
		}
	}
}

void get_sink_info_change_cb(pa_context *c,
			     const pa_sink_info *i,
			     int eol,
			     void *data)
{
	Q_UNUSED(c);
	Q_ASSERT(i);
	Q_ASSERT(data);

	if (eol) return;

	for (int chan = 0; chan < i->channel_map.channels; chan++) {
		PaClient *self = reinterpret_cast<PaClient*>(data);
		QHash<int, pa_cvolume *> states = self->sink_states();
		pa_cvolume *cvolume = states.value(i->index);
		// Check each channel for volume change
		if (cvolume->values[chan] != i->volume.values[chan]) {
			// On change, update cache and signal
			cvolume->values[chan] = i->volume.values[chan];
			emit self->volumeExternallyChanged(C_SINK, i->index, chan, i->volume.values[chan]);
		}
	}
}

void get_source_info_change_cb(pa_context *c,
			       const pa_source_info *i,
			       int eol,
			       void *data)
{
	Q_UNUSED(c);
	Q_ASSERT(i);
	Q_ASSERT(data);

	if (eol) return;

	for (int chan = 0; chan < i->channel_map.channels; chan++) {
		PaClient *self = reinterpret_cast<PaClient*>(data);
		QHash<int, pa_cvolume *> states = self->source_states();
		pa_cvolume *cvolume = states.value(i->index);
		// Check each channel for volume change
		if (cvolume->values[chan] != i->volume.values[chan]) {
			// On change, update cache and signal
			cvolume->values[chan] = i->volume.values[chan];
			emit self->volumeExternallyChanged(C_SOURCE, i->index, chan, i->volume.values[chan]);
		}
	}
}


void subscribe_cb(pa_context *c,
		  pa_subscription_event_type_t type,
		  uint32_t index,
		  void *data)
{
	pa_operation *o;

	if ((type & PA_SUBSCRIPTION_EVENT_TYPE_MASK) != PA_SUBSCRIPTION_EVENT_CHANGE) {
		qWarning("PaClient: unhandled subscribe event operation");
		return;
	}

	switch (type & PA_SUBSCRIPTION_EVENT_FACILITY_MASK) {
		case PA_SUBSCRIPTION_EVENT_SINK:
			if (!(o = pa_context_get_sink_info_by_index(c, index, get_sink_info_change_cb, data))) {
				qWarning() << "PaClient: get sink info by index: " <<
					pa_strerror(pa_context_errno(c));
				return;
			}
			break;
		case PA_SUBSCRIPTION_EVENT_SOURCE:
			if (!(o = pa_context_get_source_info_by_index(c, index, get_source_info_change_cb, data))) {
				qWarning() << "PaClient: get source info by index: " <<
					pa_strerror(pa_context_errno(c));
				return;
			}
			break;
		default:
			qWarning("PaClient: unhandled subscribe event facility");
	}
}

void context_state_cb(pa_context *c, void *data)
{
	pa_operation *o;
	PaClient *self = reinterpret_cast<PaClient*>(data);

	switch (pa_context_get_state(c)) {
		case PA_CONTEXT_CONNECTING:
		case PA_CONTEXT_AUTHORIZING:
		case PA_CONTEXT_SETTING_NAME:
			break;
		case PA_CONTEXT_READY:
			// Fetch the controls of interest
			if (!(o = pa_context_get_source_info_list(c, get_source_list_cb, data))) {
				qWarning() << "PaClient: get source info list: " <<
					pa_strerror(pa_context_errno(c));
				return;
			}
			pa_operation_unref(o);
			if (!(o = pa_context_get_sink_info_list(c, &get_sink_list_cb, data))) {
				qWarning() << "PaClient: get sink info list: " <<
					pa_strerror(pa_context_errno(c));
				return;
			}
			pa_operation_unref(o);
			pa_context_set_subscribe_callback(c, subscribe_cb, data);
			if (!(o = pa_context_subscribe(c, (pa_subscription_mask_t)(PA_SUBSCRIPTION_MASK_SINK | PA_SUBSCRIPTION_MASK_SOURCE), NULL, NULL))) {
				qWarning() << "PaClient: subscribe: " <<
					pa_strerror(pa_context_errno(c));
				return;
			}
			break;
		case PA_CONTEXT_TERMINATED:
			self->close();
			break;

		case PA_CONTEXT_FAILED:
		default:
			qCritical() << "PaClient: connection failed: " <<
				pa_strerror(pa_context_errno(c));
			self->close();
			break;
	}
}

void PaClient::init()
{
	m_ml = pa_threaded_mainloop_new();
	if (!m_ml) {
		qCritical("PaClient: failed to create mainloop");
		return;
	}

	pa_threaded_mainloop_set_name(m_ml, "PaClient mainloop");

	m_mlapi = pa_threaded_mainloop_get_api(m_ml);

	lock();

	m_ctx = pa_context_new(m_mlapi, "Mixer");
	if (!m_ctx) {
		qCritical("PaClient: failed to create context");
		pa_threaded_mainloop_free(m_ml);
		return;
	}
	pa_context_set_state_callback(m_ctx, context_state_cb, this);

	if (pa_context_connect(m_ctx, 0, (pa_context_flags_t)0, 0) < 0) {
		qCritical("PaClient: failed to connect");
		pa_context_unref(m_ctx);
		pa_threaded_mainloop_free(m_ml);
		return;
	}

	if (pa_threaded_mainloop_start(m_ml) != 0) {
		qCritical("PaClient: failed to start mainloop");
		pa_context_unref(m_ctx);
		pa_threaded_mainloop_free(m_ml);
		return;
	}

	unlock();

	m_init = true;
}

void PaClient::addOneControlState(int type, int index, const pa_cvolume *cvolume)
{
	pa_cvolume *cvolume_new = new pa_cvolume;
	cvolume_new->channels = cvolume->channels;
	for (int i = 0; i < cvolume->channels; i++)
		cvolume_new->values[i] = cvolume->values[i];
	if (type == C_SINK)
		m_sink_states.insert(index, cvolume_new);
	else if (type == C_SOURCE)
		m_source_states.insert(index, cvolume_new);
}

QHash<int, pa_cvolume *> PaClient::sink_states(void)
{
	return m_sink_states;
}

QHash<int, pa_cvolume *> PaClient::source_states(void)
{
	return m_source_states;
}
