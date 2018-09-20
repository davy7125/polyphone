//=============================================================================
//  MuseScore
//  Linux Music Score Editor
//  $Id:$
//
//  Copyright (C) 2010 Werner Schweer and others
//                2015 Davy Triponney
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License version 2.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//=============================================================================

#ifndef __SOUNDFONT_H__
#define __SOUNDFONT_H__

#include <QtCore/QString>
#include <QtCore/QList>
class QFile;

namespace SfConvert {
class Xml;

//---------------------------------------------------------
//   sfVersionTag
//---------------------------------------------------------

struct sfVersionTag {
    int major;
    int minor;
};

enum Modulator {
};
enum Generator {
    Gen_StartAddrOfs, Gen_EndAddrOfs, Gen_StartLoopAddrOfs,
    Gen_EndLoopAddrOfs, Gen_StartAddrCoarseOfs, Gen_ModLFO2Pitch,
    Gen_VibLFO2Pitch, Gen_ModEnv2Pitch, Gen_FilterFc, Gen_FilterQ,
    Gen_ModLFO2FilterFc, Gen_ModEnv2FilterFc, Gen_EndAddrCoarseOfs,
    Gen_ModLFO2Vol, Gen_Unused1, Gen_ChorusSend, Gen_ReverbSend, Gen_Pan,
    Gen_Unused2, Gen_Unused3, Gen_Unused4,
    Gen_ModLFODelay, Gen_ModLFOFreq, Gen_VibLFODelay, Gen_VibLFOFreq,
    Gen_ModEnvDelay, Gen_ModEnvAttack, Gen_ModEnvHold, Gen_ModEnvDecay,
    Gen_ModEnvSustain, Gen_ModEnvRelease, Gen_Key2ModEnvHold,
    Gen_Key2ModEnvDecay, Gen_VolEnvDelay, Gen_VolEnvAttack,
    Gen_VolEnvHold, Gen_VolEnvDecay, Gen_VolEnvSustain, Gen_VolEnvRelease,
    Gen_Key2VolEnvHold, Gen_Key2VolEnvDecay, Gen_Instrument,
    Gen_Reserved1, Gen_KeyRange, Gen_VelRange,
    Gen_StartLoopAddrCoarseOfs, Gen_Keynum, Gen_Velocity,
    Gen_Attenuation, Gen_Reserved2, Gen_EndLoopAddrCoarseOfs,
    Gen_CoarseTune, Gen_FineTune, Gen_SampleId, Gen_SampleModes,
    Gen_Reserved3, Gen_ScaleTune, Gen_ExclusiveClass, Gen_OverrideRootKey,
    Gen_Dummy
};

enum Transform { Linear };

//---------------------------------------------------------
//   ModulatorList
//---------------------------------------------------------

struct ModulatorList {
    Modulator src;
    Generator dst;
    int amount;
    Modulator amtSrc;
    Transform transform;
};

//---------------------------------------------------------
//   GeneratorList
//---------------------------------------------------------

union GeneratorAmount {
    short sword;
    ushort uword;
    struct {
        uchar lo, hi;
    };
};

struct GeneratorList {
    Generator gen;
    GeneratorAmount amount;
};

//---------------------------------------------------------
//   Zone
//---------------------------------------------------------

struct Zone {
    QList<GeneratorList*> generators;
    QList<ModulatorList*> modulators;
    int instrumentIndex;
};

//---------------------------------------------------------
//   Preset
//---------------------------------------------------------

struct Preset {
    char* name;
    int preset;
    int bank;
    int presetBagNdx; // used only for read
    int library;
    int genre;
    int morphology;
    QList<Zone*> zones;

    Preset():name(0), preset(0), bank(0), presetBagNdx(0), library(0), genre(0), morphology(0) {}
};

//---------------------------------------------------------
//   Instrument
//---------------------------------------------------------

struct Instrument {
    char* name;
    int index;        // used only for read
    QList<Zone*> zones;

    Instrument();
    ~Instrument();
};

//---------------------------------------------------------
//   Sample
//---------------------------------------------------------

struct Sample {
    char* name;
    uint start;
    uint end;
    uint loopstart;
    uint loopend;
    uint samplerate;

    int origpitch;
    int pitchadj;
    int sampleLink;
    int sampletype;

    Sample();
    ~Sample();
};

//---------------------------------------------------------
//   SoundFont
//---------------------------------------------------------

class SoundFont {
    QString _path;
    sfVersionTag _version;
    char* _engine;
    char* _name;
    char* _date;
    char* _comment;
    char* _tools;
    char* _creator;
    char* _product;
    char* _copyright;

    int _samplePos;
    int _sampleLen;

    QList<Preset*> _presets;
    QList<Instrument*> _instruments;

    QList<Zone*> _pZones;
    QList<Zone*> _iZones;
    QList<Sample*> _samples;

    QFile* _file;
    FILE* _f;

    bool _smallSf;

    unsigned readDword();
    int readWord();
    int readShort();
    int readByte();
    int readChar();
    int readFourcc(const char* signature);
    int readFourcc(char* signature);
    void readSignature(const char* signature);
    void readSignature(char* signature);
    void skip(int n);
    void readSection(const char* fourcc, int len);
    void readVersion();
    char* readString(int n);
    void readPhdr(int len);
    void readBag(int, QList<Zone*>* zones);
    void readMod(int, QList<Zone*>* zones);
    void readGen(int, QList<Zone*>* zones);
    void readInst(int size);
    void readShdr(int size);

    void writeDword(int val);
    void writeWord(unsigned short int val);
    void writeByte(unsigned char val);
    void writeChar(char val);
    void writeShort(short val);
    void write(const char* p, int n);
    void writeSample(const Sample* s);
    void writeStringSection(const char* fourcc, char* s);
    void writePreset(int zoneIdx, const Preset* preset);
    void writeModulator(const ModulatorList* m);
    void writeGenerator(const GeneratorList* g);
    void writeInstrument(int zoneIdx, const Instrument* instrument);

    void writeIfil();
    void writeSmpl(int quality);
    void writePhdr();
    void writeBag(const char* fourcc, QList<Zone*>* zones);
    void writeMod(const char* fourcc, const QList<Zone*>* zones);
    void writeGen(const char* fourcc, QList<Zone*>* zones);
    void writeInst();
    void writeShdr();

    int writeCompressedSample(Sample* s, int quality);
    bool writeCSample(Sample*, int idx);

public:
    SoundFont(const QString& s);
    ~SoundFont();
    bool read();
    bool write(QFile * f, int quality);
};
}

#endif

