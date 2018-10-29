#ifndef RUNNABLESAMPLECREATOR_H
#define RUNNABLESAMPLECREATOR_H

#include <QRunnable>
#include "basetypes.h"
#include "toolmixturecreation_parameters.h"
class ToolMixtureCreation;

class RunnableSampleCreator: public QRunnable
{
public:
    RunnableSampleCreator(ToolMixtureCreation * tool, EltID idInst, DivisionInfo di, int key, int minKey, bool loop, bool stereo, int side);
    ~RunnableSampleCreator();
    void run() override;

private:
    EltID closestSample(EltID idInst, double pitch, double &ecart, int cote, EltID &idInstSmpl);
    QByteArray getSampleData(EltID idSmpl, qint32 nbRead);
    void addSampleData(QByteArray &baData1, QByteArray &baData2, double mult);

    static double SAMPLE_DURATION; // In seconds
    static int SAMPLE_RATE; // In samples per second

    ToolMixtureCreation * _tool;
    EltID _idInst;
    DivisionInfo _di;
    int _key, _minKey;
    bool _loop;
    bool _stereo;
    int _side;

    static QMutex _mutex;
};

#endif // RUNNABLESAMPLECREATOR_H
