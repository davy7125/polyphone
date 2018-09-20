#ifndef MIDIDEVICE_H
#define MIDIDEVICE_H

#include <QString>
#include <QStringList>
#include <QObject>
class ConfManager;
class RtMidiIn;

class MidiDevice: public QObject
{
public:
    MidiDevice(ConfManager * configuration);
    ~MidiDevice();

    QStringList getMidiList();
    void openMidiPort(int index);

public slots:

private:
    ConfManager * _configuration;
    RtMidiIn * _midiin;
};

#endif // MIDIDEVICE_H
