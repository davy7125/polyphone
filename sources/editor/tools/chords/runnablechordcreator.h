#ifndef RUNNABLECHORDCREATOR_H
#define RUNNABLECHORDCREATOR_H

#include <QRunnable>
#include "basetypes.h"
#include "toolchords_parameters.h"
class ToolChords;

class RunnableChordCreator: public QRunnable
{
public:
    RunnableChordCreator(ToolChords * tool, EltID idInst, ChordInfo ci, int key, int minKey, bool loop, bool stereo, int side);
    ~RunnableChordCreator();
    void run() override;

private:
    EltID closestSample(EltID idInst, double pitch, double &ecart, int cote, EltID &idInstSmpl);
    QByteArray getSampleData(EltID idSmpl, qint32 nbRead);
    void addSampleData(QByteArray &baData1, QByteArray &baData2, double mult);
    static QMap<int, int> getChordKeys(int key, ChordInfo& chordInfo);

    static double SAMPLE_DURATION; // In seconds
    static int SAMPLE_RATE; // In samples per second

    ToolChords * _tool;
    EltID _idInst;
    ChordInfo _ci;
    int _key, _minKey;
    bool _loop;
    bool _stereo;
    int _side;
};

#endif // RUNNABLECHORDCREATOR_H
