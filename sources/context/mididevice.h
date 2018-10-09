#ifndef MIDIDEVICE_H
#define MIDIDEVICE_H

#include <QString>
#include <QStringList>
#include <QObject>
class ConfManager;
class RtMidiIn;
class PianoKeybdCustom;
class Synth;

class MidiDevice: public QObject
{
    Q_OBJECT

public:
    MidiDevice(ConfManager * configuration, Synth * synth);
    ~MidiDevice();

    // Initialize the midi device
    QStringList getMidiList();
    void openMidiPort(int index);

    // Connect the keyboard
    void setKeyboard(PianoKeybdCustom * keyboard);
    PianoKeybdCustom * keyboard() { return _keyboard; }

    // Stop all keys
    void stopAll();

public slots:
    void setKey(int key, int vel, bool syncKeyboard = false);
    void setKeyOff(int key, bool syncKeyboard = false);
    void setController(int numController, int value);

signals:
    void keyPlayed(int key, int vel);

protected:
    void customEvent(QEvent * event);

private:
    PianoKeybdCustom * _keyboard;
    ConfManager * _configuration;
    RtMidiIn * _midiin;
    Synth * _synth;

    // Sustain pedal
    QList<int> _listKeysToRelease;
    bool _isSustainOn;
};

#endif // MIDIDEVICE_H
