//=============================================================================
//  MuseScore
//  Linux Music Score Editor
//  $Id:$
//
//  Copyright (C) 2010 Werner Schweer and others
//
//  This work is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Library General Public
//  License as published by the Free Software Foundation; either
//  version 2 of the License, or (at your option) any later version.
//
//  In addition, as a special exception, licensor gives permission to
//  link the code of this work with the OpenSSL Library (or with modified
//  versions of OpenSSL that use the same license as OpenSSL), and
//  distribute linked combinations including the two. You must obey the
//  GNU General Public License in all respects for all of the code used
//  other than OpenSSL. If you modify this file, you may extend this
//  exception to your version of the file, but you are not obligated to
//  do so. If you do not wish to do so, delete this exception statement
//  from your version. (This exception is necessary should this work be
//  included in a GPL-licenced work.)
//
//  See COPYING.LIB for the licence text and disclaimer of warranty.
//=============================================================================

#ifndef SFONT_H
#define SFONT_H

#include <QtCore/QString>
#include <QtCore/QList>

class Xml;
class QFile;

namespace SfTools {

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

      Preset():name(nullptr), preset(0), bank(0), presetBagNdx(0), library(0), genre(0), morphology(0) {}
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
      QString path;
      sfVersionTag version;
      char* engine;
      char* name;
      char* date;
      char* comment;
      char* tools;
      char* creator;
      char* product;
      char* copyright;
      char* irom;
      sfVersionTag iver;

      int samplePos;
      int sampleLen;

      QList<Preset*> presets;
      QList<Instrument*> instruments;

      QList<Zone*> pZones;
      QList<Zone*> iZones;
      QList<Sample*> samples;

      QFile* file;
      FILE* f;

      bool _compress;
      double _oggQuality;
      double _oggAmp;
      qint64 _oggSerial;

      // Extra option
      bool _smallSf;

      unsigned readDword();
      int readWord();
      int readShort();
      int readByte();
      int readChar();
      int readFourcc(const char*);
      int readFourcc(char*);
      void readSignature(const char* signature);
      void readSignature(char* signature);
      void skip(int);
      void readSection(const char* fourcc, int len);
      void readVersion(sfVersionTag* v);
      char* readString(int);
      void readPhdr(int);
      void readBag(int, QList<Zone*>*);
      void readMod(int, QList<Zone*>*);
      void readGen(int, QList<Zone*>*);
      void readInst(int);
      void readShdr(int);

      void writeDword(int);
      void writeWord(unsigned short int);
      void writeByte(unsigned char);
      void writeChar(char);
      void writeShort(short);
      void write(const char* p, int n);
      void writeSample(const Sample*);
      void writeStringSection(const char* fourcc, char* s);
      void writePreset(int zoneIdx, const Preset*);
      void writeModulator(const ModulatorList*);
      void writeGenerator(const GeneratorList*);
      void writeInstrument(int zoneIdx, const Instrument*);

      void writeIfil();
      void writeIver();
      void writeSmpl();
      void writePhdr();
      void writeBag(const char* fourcc, QList<Zone*>*);
      void writeMod(const char* fourcc, const QList<Zone*>*);
      void writeGen(const char* fourcc, QList<Zone*>*);
      void writeInst();
      void writeShdr();

      int writeCompressedSample(Sample*);
      int writeUncompressedSample(Sample* s);
      bool writeCSample(Sample*, int);
      bool write();

   public:
      SoundFont(const QString&);
      ~SoundFont();
      bool read();
      bool compress(QFile* f, double oggQuality, double oggAmp, qint64 oggSerial = rand());
      bool uncompress(QFile* f);
      bool writeCode(QList<int>);
      bool writeCode();
      void dumpPresets();

#ifndef SFTOOLS_NOXML
    private:
      void write(Xml&, Zone*);
      bool writeSampleFile(Sample*, QString);

    public:
      bool readXml(QFile*);
      bool writeXml(QFile*);
#endif
};
}
#endif

