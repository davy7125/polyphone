/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2024 Davy Triponney                                **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program. If not, see http://www.gnu.org/licenses/.    **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: https://www.polyphone.io                             **
**             Date: 01.01.2013                                           **
***************************************************************************/


#include "clap_instance.h"
#include <clap/clap.h>
#include <dlfcn.h>
#include <cstring>

struct ClapInstance::Private
{
    void * library = nullptr;
    const clap_plugin_entry_t * entry = nullptr;
    const clap_plugin_t * plugin = nullptr;
    clap_host_t host;
    bool isActive = false;
    bool isProcessing = false;
    bool callbackRequested = false;
    int64_t steadyTime = 0;

    // Events of the block being computed
    std::vector<clap_event_midi_t> midiEvents;
};

static const void * hostGetExtension(const clap_host_t *, const char *)
{
    return nullptr;
}

static void hostRequestRestart(const clap_host_t *) {}
static void hostRequestProcess(const clap_host_t *) {}

static void hostRequestCallback(const clap_host_t * host)
{
    static_cast<ClapInstance::Private *>(host->host_data)->callbackRequested = true;
}

static uint32_t eventsSize(const clap_input_events_t * events)
{
    return static_cast<uint32_t>(static_cast<ClapInstance::Private *>(events->ctx)->midiEvents.size());
}

static const clap_event_header_t * eventsGet(const clap_input_events_t * events, uint32_t index)
{
    return &static_cast<ClapInstance::Private *>(events->ctx)->midiEvents[index].header;
}

static bool outputEventsTryPush(const clap_output_events_t *, const clap_event_header_t *)
{
    return true;
}

ClapInstance::ClapInstance() :
    _private(new Private())
{
    clap_host_t &host = _private->host;
    host.clap_version = CLAP_VERSION;
    host.host_data = _private;
    host.name = "Polyphone plugin tests";
    host.vendor = "Polyphone";
    host.url = "https://www.polyphone.io";
    host.version = "1.0";
    host.get_extension = hostGetExtension;
    host.request_restart = hostRequestRestart;
    host.request_process = hostRequestProcess;
    host.request_callback = hostRequestCallback;
}

ClapInstance::~ClapInstance()
{
    deactivate();
    if (_private->plugin != nullptr)
        _private->plugin->destroy(_private->plugin);
    if (_private->entry != nullptr)
        _private->entry->deinit();
    if (_private->library != nullptr)
        dlclose(_private->library);
    delete _private;
}

bool ClapInstance::load(const std::string &path, std::string &error)
{
    _private->library = dlopen(path.c_str(), RTLD_NOW | RTLD_LOCAL);
    if (_private->library == nullptr)
    {
        error = dlerror();
        return false;
    }
    _private->entry = static_cast<const clap_plugin_entry_t *>(dlsym(_private->library, "clap_entry"));
    if (_private->entry == nullptr || !_private->entry->init(path.c_str()))
    {
        _private->entry = nullptr;
        error = "no entry";
        return false;
    }
    const clap_plugin_factory_t * factory = static_cast<const clap_plugin_factory_t *>(
        _private->entry->get_factory(CLAP_PLUGIN_FACTORY_ID));
    if (factory == nullptr || factory->get_plugin_count(factory) < 1)
    {
        error = "no plugin";
        return false;
    }
    const clap_plugin_descriptor_t * descriptor = factory->get_plugin_descriptor(factory, 0);
    _private->plugin = factory->create_plugin(factory, &_private->host, descriptor->id);
    if (_private->plugin == nullptr || !_private->plugin->init(_private->plugin))
    {
        error = "cannot create the plugin";
        return false;
    }
    return true;
}

namespace
{
struct Stream
{
    std::vector<uint8_t> * data;
    size_t position;
};
}

static int64_t streamRead(const clap_istream_t * stream, void * buffer, uint64_t size)
{
    Stream * state = static_cast<Stream *>(stream->ctx);
    uint64_t count = std::min<uint64_t>(size, state->data->size() - state->position);
    memcpy(buffer, state->data->data() + state->position, count);
    state->position += count;
    return static_cast<int64_t>(count);
}

static int64_t streamWrite(const clap_ostream_t * stream, const void * buffer, uint64_t size)
{
    Stream * state = static_cast<Stream *>(stream->ctx);
    const uint8_t * bytes = static_cast<const uint8_t *>(buffer);
    state->data->insert(state->data->end(), bytes, bytes + size);
    return static_cast<int64_t>(size);
}

bool ClapInstance::setState(const std::vector<uint8_t> &state)
{
    const clap_plugin_state_t * extension = static_cast<const clap_plugin_state_t *>(
        _private->plugin->get_extension(_private->plugin, CLAP_EXT_STATE));
    if (extension == nullptr)
        return false;
    std::vector<uint8_t> copy = state;
    Stream context { &copy, 0 };
    clap_istream_t stream { &context, streamRead };
    return extension->load(_private->plugin, &stream);
}

bool ClapInstance::getState(std::vector<uint8_t> &state)
{
    const clap_plugin_state_t * extension = static_cast<const clap_plugin_state_t *>(
        _private->plugin->get_extension(_private->plugin, CLAP_EXT_STATE));
    if (extension == nullptr)
        return false;
    state.clear();
    Stream context { &state, 0 };
    clap_ostream_t stream { &context, streamWrite };
    return extension->save(_private->plugin, &stream);
}

bool ClapInstance::activate(double sampleRate, uint32_t maxFrameCount, bool isOffline)
{
    deactivate();
    const clap_plugin_render_t * render = static_cast<const clap_plugin_render_t *>(
        _private->plugin->get_extension(_private->plugin, CLAP_EXT_RENDER));
    if (render != nullptr)
        render->set(_private->plugin, isOffline ? CLAP_RENDER_OFFLINE : CLAP_RENDER_REALTIME);
    _private->isActive = _private->plugin->activate(_private->plugin, sampleRate, 1, maxFrameCount);
    return _private->isActive;
}

void ClapInstance::deactivate()
{
    if (_private->isProcessing)
    {
        _private->plugin->stop_processing(_private->plugin);
        _private->isProcessing = false;
    }
    if (_private->isActive)
    {
        _private->plugin->deactivate(_private->plugin);
        _private->isActive = false;
    }
}

void ClapInstance::process(float * left, float * right, uint32_t frameCount, const std::vector<MidiEvent> &events)
{
    if (!_private->isProcessing)
        _private->isProcessing = _private->plugin->start_processing(_private->plugin);

    _private->midiEvents.clear();
    for (const MidiEvent &event : events)
    {
        clap_event_midi_t midi;
        midi.header.size = sizeof(clap_event_midi_t);
        midi.header.time = event.position;
        midi.header.space_id = CLAP_CORE_EVENT_SPACE_ID;
        midi.header.type = CLAP_EVENT_MIDI;
        midi.header.flags = 0;
        midi.port_index = 0;
        memcpy(midi.data, event.data, 3);
        _private->midiEvents.push_back(midi);
    }
    clap_input_events_t inputEvents { _private, eventsSize, eventsGet };
    clap_output_events_t outputEvents { _private, outputEventsTryPush };

    float * channels[2] = { left, right };
    clap_audio_buffer_t output;
    output.data32 = channels;
    output.data64 = nullptr;
    output.channel_count = 2;
    output.latency = 0;
    output.constant_mask = 0;

    clap_process_t process;
    memset(&process, 0, sizeof(process));
    process.steady_time = _private->steadyTime;
    process.frames_count = frameCount;
    process.audio_outputs = &output;
    process.audio_outputs_count = 1;
    process.in_events = &inputEvents;
    process.out_events = &outputEvents;
    _private->plugin->process(_private->plugin, &process);
    _private->steadyTime += frameCount;

    // Requests of the plugin for the main thread (the tests compute in the main thread)
    if (_private->callbackRequested)
    {
        _private->callbackRequested = false;
        _private->plugin->on_main_thread(_private->plugin);
    }
}
