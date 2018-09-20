#include "mididevice.h"
#include "RtMidi.h"
#include "pianokeybdcustom.h"
#include <QEvent>
#include <QApplication>

// Callback for MIDI signals
void midiCallback(double deltatime, std::vector< unsigned char > *message, void *userData)
{
    Q_UNUSED(deltatime);
    // Constantes
    int STATUS_NOTEOFF    = 0x80;
    int STATUS_NOTEON     = 0x90;
    int STATUS_CONTROLLER = 0xB0;
    //int MASK_CHANNEL   = 0x0f;
    int MASK_STATUS    = 0xF0;
    // Récupération de l'instance de PianoKeybdCustom
    PianoKeybdCustom * instance = static_cast<PianoKeybdCustom*>(userData);
    QEvent* ev = 0;
    //unsigned char channel = message->at(0) & MASK_CHANNEL;
    unsigned char status = message->at(0) & MASK_STATUS;
    if (status == STATUS_NOTEON || status == STATUS_NOTEOFF)
    {
        unsigned char midi_note = message->at(1);
        unsigned char vel = message->at(2);
        if ((status == STATUS_NOTEOFF) || (vel == 0))
            ev = new NoteEvent(midi_note, 0);
        else
            ev = new NoteEvent(midi_note, vel);
    }
    else if (status == STATUS_CONTROLLER)
    {
        unsigned char numController = message->at(1);
        unsigned char value = message->at(2);
        ev = new ControllerEvent(numController, value);
    }
    if (ev)
        QApplication::postEvent(instance, ev);
}

MidiDevice::MidiDevice(ConfManager * configuration) :
    _configuration(configuration)
{
    // Connexion midi
    try
    {
#if defined(__LINUX_ALSASEQ__)
        _midiin = new RtMidiIn(RtMidi::LINUX_ALSA, "Polyphone");
#endif
#if defined(__WINDOWS_MM__)
        _midiin = new RtMidiIn(RtMidi::WINDOWS_MM, "Polyphone");
#endif
#if defined(__MACOSX_CORE__)
        _midiin = new RtMidiIn(RtMidi::MACOSX_CORE, "Polyphone");
#endif
    }
    catch (std::exception &error)
    {
        Q_UNUSED(error)
        _midiin = NULL;
    }
    if (_midiin)
    {
        _midiin->ignoreTypes(false, false, false);
        _midiin->setCallback(&midiCallback, this);
    }

    connect(this, SIGNAL(noteOn(int,int)), this, SLOT(setKey(int,int)));
    connect(this, SIGNAL(noteOff(int)), this, SLOT(setKeyOff(int)));
}

MidiDevice::~MidiDevice()
{
    delete _midiin;
}

QStringList MidiDevice::getMidiList()
{

    QStringList listRet;
    if (_midiin)
    {
        for (unsigned int i = 0; i < _midiin->getPortCount(); i++)
            listRet.append(QString(_midiin->getPortName(i).c_str()));
    }
    return listRet;
}


void MidiDevice::openMidiPort(int index)
{
    if (_midiin)
    {
        _midiin->closePort();
        if (index < (signed)_midiin->getPortCount() && index != -1)
        {
            try
            {
                _midiin->openPort(index);
            }
            catch (std::exception &error)
            {
                Q_UNUSED(error)
            }
        }
    }
}
