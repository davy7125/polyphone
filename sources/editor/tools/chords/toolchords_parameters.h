#ifndef TOOLCHORDS_PARAMETERS_H
#define TOOLCHORDS_PARAMETERS_H

#include "abstracttoolparameters.h"
#include <QList>
#include <QString>
#include "qmath.h"


class ChordInfo
{
public:
    int chordType3;
    int chordType5;
    int chordType7;
    int chordType9;
    int harmonicNumber;
    double attenuation;
    int position;
    int rootNote; // -1 is rootkey, 0 is C, 1 is C#, 2 is D, ..., 11 is B
};

class ToolChords_parameters: public AbstractToolParameters
{
public:
    /// Load the configuration from the ini file
    void loadConfiguration() override;

    /// Save the configuration in the ini file
    void saveConfiguration() override;

    QString getInstrumentName() { return _instrumentName; }
    void setInstrumentName(QString instrumentName) { _instrumentName = instrumentName; }
    
    int getDensityType() { return _densityType; }
    void setDensityType(int densityType) { _densityType = densityType; }
    
    bool getStereoSample() { return _stereoSample; }
    void setStereoSample(bool stereoSample) { _stereoSample = stereoSample; }
    
    bool getLoopSample() { return _loopSample; }
    void setLoopSample(bool loopSample) { _loopSample = loopSample; }
    
    ChordInfo & getChordConfiguration() { return _chordInfo; }
    void setChordConfiguration(ChordInfo chordInfo) { _chordInfo = chordInfo; }
    
private:
    QString _instrumentName;
    int _densityType;
    bool _stereoSample;
    bool _loopSample;
    ChordInfo _chordInfo;
};

#endif // TOOLCHORDS_PARAMETERS_H
