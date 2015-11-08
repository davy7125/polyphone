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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vorbis/vorbisenc.h>
#include <QFile>

#include "sfont.h"
#include "time.h"

#define printf(fmt, ...) (0)

using namespace SfConvert;

// #define DEBUG

#define BE_SHORT(x) ((((x)&0xFF)<<8) | (((x)>>8)&0xFF))
#ifdef __i486__
#define BE_LONG(x) \
    ({ int __value; \
    asm ("bswap %1; movl %1,%0" : "=g" (__value) : "r" (x)); \
    __value; })
#else
#define BE_LONG(x) ((((x)&0xFF)<<24) | \
    (((x)&0xFF00)<<8) | \
    (((x)&0xFF0000)>>8) | \
    (((x)>>24)&0xFF))
#endif

#define FOURCC(a, b, c, d) a << 24 | b << 16 | c << 8 | d

#define BLOCK_SIZE 1024

static const bool writeCompressed = true;

//---------------------------------------------------------
//   Sample
//---------------------------------------------------------

Sample::Sample()
{
    name = 0;
}

Sample::~Sample()
{
    free(name);
}

//---------------------------------------------------------
//   Instrument
//---------------------------------------------------------

Instrument::Instrument()
{
    name = 0;
}

Instrument::~Instrument()
{
    free(name);
}

//---------------------------------------------------------
//   SoundFont
//---------------------------------------------------------

SoundFont::SoundFont(const QString& s)
{
    _path      = s;
    _engine    = 0;
    _name      = 0;
    _date      = 0;
    _comment   = 0;
    _tools     = 0;
    _creator   = 0;
    _product   = 0;
    _copyright = 0;

#ifdef SF_SMALL
    _smallSf = true;
#else
    _smallSf = false;
#endif
}

SoundFont::~SoundFont()
{
    free(_engine);
    free(_name);
    free(_date);
    free(_comment);
    free(_tools);
    free(_creator);
    free(_product);
    free(_copyright);
}

//---------------------------------------------------------
//   read
//---------------------------------------------------------

bool SoundFont::read()
{
    _file = new QFile(_path);
    if (!_file->open(QIODevice::ReadOnly)) {
        fprintf(stderr, "cannot open <%s>\n", qPrintable(_path));
        delete _file;
        return false;
    }
    try {
        int len = readFourcc("RIFF");
        readSignature("sfbk");
        len -= 4;
        while (len) {
            int len2 = readFourcc("LIST");
            len -= (len2 + 8);
            char fourcc[5];
            fourcc[0] = 0;
            readSignature(fourcc);
            fourcc[4] = 0;
            len2 -= 4;
            while (len2) {
                fourcc[0] = 0;
                int len3 = readFourcc(fourcc);
                fourcc[4] = 0;
                len2 -= (len3 + 8);
                readSection(fourcc, len3);
            }
        }
    }
    catch (QString s) {
        fprintf(stderr, "read sf file failed: %s\n", qPrintable(s));
        delete _file;
        return false;
    }
    delete _file;
    return true;
}

//---------------------------------------------------------
//   skip
//---------------------------------------------------------

void SoundFont::skip(int n)
{
    qint64 pos = _file->pos();
    if (!_file->seek(pos + n))
        throw(QString("unexpected end of file\n"));
}

//---------------------------------------------------------
//   readFourcc
//---------------------------------------------------------

int SoundFont::readFourcc(char* signature)
{
    readSignature(signature);
    return readDword();
}

int SoundFont::readFourcc(const char* signature)
{
    readSignature(signature);
    return readDword();
}

//---------------------------------------------------------
//   readSignature
//---------------------------------------------------------

void SoundFont::readSignature(const char* signature)
{
    char fourcc[4];
    readSignature(fourcc);
    if (memcmp(fourcc, signature, 4) != 0)
        throw(QString("fourcc <%1> expected").arg(signature));
}

void SoundFont::readSignature(char* signature)
{
    if (_file->read(signature, 4) != 4)
        throw(QString("unexpected end of file\n"));
}

//---------------------------------------------------------
//   readDword
//---------------------------------------------------------

unsigned SoundFont::readDword()
{
    unsigned format;
    if (_file->read((char*)&format, 4) != 4)
        throw(QString("unexpected end of file\n"));
    if (QSysInfo::ByteOrder == QSysInfo::BigEndian)
        return BE_LONG(format);
    else
        return format;
}

//---------------------------------------------------------
//   writeDword
//---------------------------------------------------------

void SoundFont::writeDword(int val)
{
    if (QSysInfo::ByteOrder == QSysInfo::BigEndian)
        val = BE_LONG(val);
    write((char*)&val, 4);
}

//---------------------------------------------------------
//   writeWord
//---------------------------------------------------------

void SoundFont::writeWord(unsigned short int val)
{
    if (QSysInfo::ByteOrder == QSysInfo::BigEndian)
        val = BE_SHORT(val);
    write((char*)&val, 2);
}

//---------------------------------------------------------
//   writeByte
//---------------------------------------------------------

void SoundFont::writeByte(unsigned char val)
{
    write((char*)&val, 1);
}

//---------------------------------------------------------
//   writeChar
//---------------------------------------------------------

void SoundFont::writeChar(char val)
{
    write((char*)&val, 1);
}

//---------------------------------------------------------
//   writeShort
//---------------------------------------------------------

void SoundFont::writeShort(short val)
{
    if (QSysInfo::ByteOrder == QSysInfo::BigEndian)
        val = BE_SHORT(val);
    write((char*)&val, 2);
}

//---------------------------------------------------------
//   readWord
//---------------------------------------------------------

int SoundFont::readWord()
{
    unsigned short format;
    if (_file->read((char*)&format, 2) != 2)
        throw(QString("unexpected end of file\n"));
    if (QSysInfo::ByteOrder == QSysInfo::BigEndian)
        return BE_SHORT(format);
    else
        return format;
}

//---------------------------------------------------------
//   readShort
//---------------------------------------------------------

int SoundFont::readShort()
{
    short format;
    if (_file->read((char*)&format, 2) != 2)
        throw(QString("unexpected end of file\n"));
    if (QSysInfo::ByteOrder == QSysInfo::BigEndian)
        return BE_SHORT(format);
    else
        return format;
}

//---------------------------------------------------------
//   readByte
//---------------------------------------------------------

int SoundFont::readByte()
{
    uchar val;
    if (_file->read((char*)&val, 1) != 1)
        throw(QString("unexpected end of file\n"));
    return val;
}

//---------------------------------------------------------
//   readChar
//---------------------------------------------------------

int SoundFont::readChar()
{
    char val;
    if (_file->read(&val, 1) != 1)
        throw(QString("unexpected end of file\n"));
    return val;
}

//---------------------------------------------------------
//   readVersion
//---------------------------------------------------------

void SoundFont::readVersion()
{
    unsigned char data[4];
    if (_file->read((char*)data, 4) != 4)
        throw(QString("unexpected end of file\n"));
    _version.major = data[0] + (data[1] << 8);
    _version.minor = data[2] + (data[3] << 8);
}

//---------------------------------------------------------
//   readString
//---------------------------------------------------------

char* SoundFont::readString(int n)
{
    char data[2500];
    if (_file->read((char*)data, n) != n)
        throw(QString("unexpected end of file\n"));
    if (data[n-1] != 0)
        data[n] = 0;
    return strdup(data);
}

//---------------------------------------------------------
//   readSection
//---------------------------------------------------------

void SoundFont::readSection(const char* fourcc, int len)
{
    switch(FOURCC(fourcc[0], fourcc[1], fourcc[2], fourcc[3])) {
    case FOURCC('i', 'f', 'i', 'l'):    // version
        readVersion();
        break;
    case FOURCC('I','N','A','M'):       // sound font name
        _name = readString(len);
        break;
    case FOURCC('i','s','n','g'):       // target render engine
        _engine = readString(len);
        break;
    case FOURCC('I','P','R','D'):       // product for which the bank was intended
        _product = readString(len);
        break;
    case FOURCC('I','E','N','G'): // sound designers and engineers for the bank
        _creator = readString(len);
        break;
    case FOURCC('I','S','F','T'): // SoundFont tools used to create and alter the bank
        _tools = readString(len);
        break;
    case FOURCC('I','C','R','D'): // date of creation of the bank
        _date = readString(len);
        break;
    case FOURCC('I','C','M','T'): // comments on the bank
        _comment = readString(len);
        break;
    case FOURCC('I','C','O','P'): // copyright message
        _copyright = readString(len);
        break;
    case FOURCC('s','m','p','l'): // the digital audio samples
        _samplePos = _file->pos();
        _sampleLen = len;
        skip(len);
        break;
    case FOURCC('p','h','d','r'): // preset headers
        readPhdr(len);
        break;
    case FOURCC('p','b','a','g'): // preset index list
        readBag(len, &_pZones);
        break;
    case FOURCC('p','m','o','d'): // preset modulator list
        readMod(len, &_pZones);
        break;
    case FOURCC('p','g','e','n'): // preset generator list
        readGen(len, &_pZones);
        break;
    case FOURCC('i','n','s','t'): // instrument names and indices
        readInst(len);
        break;
    case FOURCC('i','b','a','g'): // instrument index list
        readBag(len, &_iZones);
        break;
    case FOURCC('i','m','o','d'): // instrument modulator list
        readMod(len, &_iZones);
        break;
    case FOURCC('i','g','e','n'): // instrument generator list
        readGen(len, &_iZones);
        break;
    case FOURCC('s','h','d','r'): // sample headers
        readShdr(len);
        break;
    case FOURCC('i', 'r', 'o', 'm'):    // sample rom
    case FOURCC('i', 'v', 'e', 'r'):    // sample rom version
    default:
        skip(len);
        throw(QString("unknown fourcc <%1>").arg(fourcc));
        break;
    }
}

//---------------------------------------------------------
//   readPhdr
//---------------------------------------------------------

void SoundFont::readPhdr(int len)
{
    if (len < (38 * 2))
        throw(QString("phdr too short"));
    if (len % 38)
        throw(QString("phdr not a multiple of 38"));
    int n = len / 38;
    if (n <= 1) {
        skip(len);
        return;
    }
    int index1 = 0, index2;
    for (int i = 0; i < n; ++i) {
        Preset* preset       = new Preset;
        preset->name         = readString(20);
        preset->preset       = readWord();
        preset->bank         = readWord();
        index2               = readWord();
        preset->library      = readDword();
        preset->genre        = readDword();
        preset->morphology   = readDword();
        if (index2 < index1)
            throw("preset header indices not monotonic");
        if (i > 0) {
            int n = index2 - index1;
            while (n--) {
                Zone* z = new Zone;
                _presets.back()->zones.append(z);
                _pZones.append(z);
            }
        }
        index1 = index2;
        _presets.append(preset);
    }
    _presets.takeLast();
}

//---------------------------------------------------------
//   readBag
//---------------------------------------------------------

void SoundFont::readBag(int len, QList<Zone*>* zones)
{
    if (len % 4)
        throw(QString("bag size not a multiple of 4"));
    int gIndex2, mIndex2;
    int gIndex1 = readWord();
    int mIndex1 = readWord();
    len -= 4;
    foreach(Zone* zone, *zones) {
        gIndex2 = readWord();
        mIndex2 = readWord();
        len -= 4;
        if (len < 0)
            throw(QString("bag size too small"));
        if (gIndex2 < gIndex1)
            throw("generator indices not monotonic");
        if (mIndex2 < mIndex1)
            throw("modulator indices not monotonic");
        int n = mIndex2 - mIndex1;
        while (n--)
            zone->modulators.append(new ModulatorList);
        n = gIndex2 - gIndex1;
        while (n--)
            zone->generators.append(new GeneratorList);
        gIndex1 = gIndex2;
        mIndex1 = mIndex2;
    }
}

//---------------------------------------------------------
//   readMod
//---------------------------------------------------------

void SoundFont::readMod(int size, QList<Zone*>* zones)
{
    foreach(Zone* zone, *zones) {
        foreach(ModulatorList* m, zone->modulators) {
            size -= 10;
            if (size < 0)
                throw(QString("pmod size mismatch"));
            m->src           = static_cast<Modulator>(readWord());
            m->dst           = static_cast<Generator>(readWord());
            m->amount        = readShort();
            m->amtSrc        = static_cast<Modulator>(readWord());
            m->transform     = static_cast<Transform>(readWord());
        }
    }
    if (size != 10)
        throw(QString("modulator list size mismatch"));
    skip(10);
}

//---------------------------------------------------------
//   readGen
//---------------------------------------------------------

void SoundFont::readGen(int size, QList<Zone*>* zones)
{
    if (size % 4)
        throw(QString("bad generator list size"));
    foreach (Zone* zone, *zones) {
        size -= (zone->generators.size() * 4);
        if (size < 0)
            break;

        foreach (GeneratorList* gen, zone->generators) {
            gen->gen = static_cast<Generator>(readWord());
            if (gen->gen == Gen_KeyRange || gen->gen == Gen_VelRange) {
                gen->amount.lo = readByte();
                gen->amount.hi = readByte();
            }
            else if (gen->gen == Gen_Instrument)
                gen->amount.uword = readWord();
            else
                gen->amount.sword = readWord();
        }
    }
    if (size != 4)
        throw(QString("generator list size mismatch %1 != 4").arg(size));
    skip(size);
}

//---------------------------------------------------------
//   readInst
//---------------------------------------------------------

void SoundFont::readInst(int size)
{
    int n = size / 22;
    int index1 = 0, index2;
    for (int i = 0; i < n; ++i) {
        Instrument* instrument = new Instrument;
        instrument->name = readString(20);
        index2           = readWord();
        if (index2 < index1)
            throw("instrument header indices not monotonic");
        if (i > 0) {
            int n = index2 - index1;
            while (n--) {
                Zone* z = new Zone;
                _instruments.back()->zones.append(z);
                _iZones.append(z);
            }
        }
        index1 = index2;
        _instruments.append(instrument);
    }
    _instruments.takeLast();
}

//---------------------------------------------------------
//   readShdr
//---------------------------------------------------------

void SoundFont::readShdr(int size)
{
    int n = size / 46;
    for (int i = 0; i < n-1; ++i) {
        Sample* s = new Sample;
        s->name       = readString(20);
        s->start      = readDword();
        s->end        = readDword();
        s->loopstart  = readDword();
        s->loopend    = readDword();
        s->samplerate = readDword();
        s->origpitch  = readByte();
        s->pitchadj   = readChar();
        s->sampleLink = readWord();
        s->sampletype = readWord();

        s->loopstart -= s->start;
        s->loopend   -= s->start;
        // printf("readFontHeader %d %d   %d %d\n", s->start, s->end, s->loopstart, s->loopend);
        _samples.append(s);
    }
    skip(46);   // trailing record
}

//---------------------------------------------------------
//   write
//---------------------------------------------------------

bool SoundFont::write(QFile *f, int quality)
{
    _file = f;
    qint64 riffLenPos;
    qint64 listLenPos;
    try {
        _file->write("RIFF", 4);
        riffLenPos = _file->pos();
        writeDword(0);
        _file->write("sfbk", 4);

        _file->write("LIST", 4);
        listLenPos = _file->pos();
        writeDword(0);
        _file->write("INFO", 4);

        writeIfil();
        if (_name)
            writeStringSection("INAM", _name);
        if (_engine)
            writeStringSection("isng", _engine);
        if (_product)
            writeStringSection("IPRD", _product);
        if (_creator)
            writeStringSection("IENG", _creator);
        if (_tools)
            writeStringSection("ISFT", _tools);
        if (_date)
            writeStringSection("ICRD", _date);
        if (_comment)
            writeStringSection("ICMT", _comment);
        if (_copyright)
            writeStringSection("ICOP", _copyright);

        qint64 pos = _file->pos();
        _file->seek(listLenPos);
        writeDword(pos - listLenPos - 4);
        _file->seek(pos);

        _file->write("LIST", 4);
        listLenPos = _file->pos();
        writeDword(0);
        _file->write("sdta", 4);
        writeSmpl(quality);
        pos = _file->pos();
        _file->seek(listLenPos);
        writeDword(pos - listLenPos - 4);
        _file->seek(pos);

        _file->write("LIST", 4);
        listLenPos = _file->pos();
        writeDword(0);
        _file->write("pdta", 4);

        writePhdr();
        writeBag("pbag", &_pZones);
        writeMod("pmod", &_pZones);
        writeGen("pgen", &_pZones);
        writeInst();
        writeBag("ibag", &_iZones);
        writeMod("imod", &_iZones);
        writeGen("igen", &_iZones);
        writeShdr();

        pos = _file->pos();
        _file->seek(listLenPos);
        writeDword(pos - listLenPos - 4);
        _file->seek(pos);

        qint64 endPos = _file->pos();
        _file->seek(riffLenPos);
        writeDword(endPos - riffLenPos - 4);
    }
    catch (QString s) {
        fprintf(stderr, "write sf file failed: %s\n", qPrintable(s));
        return false;
    }
    return true;
}

//---------------------------------------------------------
//   write
//---------------------------------------------------------

void SoundFont::write(const char* p, int n)
{
    if (_file->write(p, n) != n)
        throw(QString("write error"));
}

//---------------------------------------------------------
//   writeStringSection
//---------------------------------------------------------

void SoundFont::writeStringSection(const char* fourcc, char* s)
{
    write(fourcc, 4);
    int nn = (int)strlen(s) + 1;
    int n = ((nn + 1) / 2) * 2;
    writeDword(n);
    write(s, nn);
    if (n - nn) {
        char c = 0;
        write(&c, 1);
    }
}

//---------------------------------------------------------
//   writeIfil
//---------------------------------------------------------

void SoundFont::writeIfil()
{
    write("ifil", 4);
    writeDword(4);
    unsigned char data[4];
    if (writeCompressed)
        _version.major = 3;
    data[0] = _version.major;
    data[1] = _version.major >> 8;
    data[2] = _version.minor;
    data[3] = _version.minor >> 8;
    write((char*)data, 4);
}

//---------------------------------------------------------
//   writeSmpl
//---------------------------------------------------------

void SoundFont::writeSmpl(int quality)
{
    write("smpl", 4);

    qint64 pos = _file->pos();
    writeDword(0);
    int sampleLen = 0;
    if (writeCompressed)
    {
        foreach (Sample* s, _samples)
        {
            s->sampletype |= 0x10; // Sf3 flag added
            int len = writeCompressedSample(s, quality);
            s->start = sampleLen;
            sampleLen += len;
            s->end = sampleLen;
        }
    }
    else
    {
        char* buffer = new char[sampleLen];
        QFile f(_path);
        if (!f.open(QIODevice::ReadOnly))
            throw(QString("cannot open <%1>").arg(f.fileName()));
        foreach(Sample* s, _samples) {
            f.seek(_samplePos + s->start * sizeof(short));

            int len = (s->end - s->start) * sizeof(short);
            f.read(buffer, len);
            write(buffer, len);
            s->start = sampleLen / sizeof(short);
            sampleLen += len;
            s->end = sampleLen / sizeof(short);
            s->loopstart += s->start;
            s->loopend   += s->start;
        }
        f.close();
        delete[] buffer;
    }
    qint64 npos = _file->pos();
    _file->seek(pos);
    writeDword(npos - pos - 4);
    _file->seek(npos);
}

//---------------------------------------------------------
//   writePhdr
//---------------------------------------------------------

void SoundFont::writePhdr()
{
    write("phdr", 4);
    int n = _presets.size();
    writeDword((n + 1) * 38);
    int zoneIdx = 0;
    foreach(const Preset* p, _presets) {
        writePreset(zoneIdx, p);
        zoneIdx += p->zones.size();
    }
    Preset p;
    writePreset(zoneIdx, &p);
}

//---------------------------------------------------------
//   writePreset
//---------------------------------------------------------

void SoundFont::writePreset(int zoneIdx, const Preset* preset)
{
    char name[20];
    memset(name, 0, 20);
    if (preset->name)
        memcpy(name, preset->name, strlen(preset->name));
    write(name, 20);
    writeWord(preset->preset);
    writeWord(preset->bank);
    writeWord(zoneIdx);
    writeDword(preset->library);
    writeDword(preset->genre);
    writeDword(preset->morphology);
}

//---------------------------------------------------------
//   writeBag
//---------------------------------------------------------

void SoundFont::writeBag(const char* fourcc, QList<Zone*>* zones)
{
    write(fourcc, 4);
    int n = zones->size();
    writeDword((n + 1) * 4);
    int gIndex = 0;
    int pIndex = 0;
    foreach(const Zone* z, *zones) {
        writeWord(gIndex);
        writeWord(pIndex);
        gIndex += z->generators.size();
        pIndex += z->modulators.size();
    }
    writeWord(gIndex);
    writeWord(pIndex);
}

//---------------------------------------------------------
//   writeMod
//---------------------------------------------------------

void SoundFont::writeMod(const char* fourcc, const QList<Zone*>* zones)
{
    write(fourcc, 4);
    int n = 0;
    foreach(const Zone* z, *zones)
        n += z->modulators.size();
    writeDword((n + 1) * 10);

    foreach(const Zone* zone, *zones) {
        foreach(const ModulatorList* m, zone->modulators)
            writeModulator(m);
    }
    ModulatorList mod;
    memset(&mod, 0, sizeof(mod));
    writeModulator(&mod);
}

//---------------------------------------------------------
//   writeModulator
//---------------------------------------------------------

void SoundFont::writeModulator(const ModulatorList* m)
{
    writeWord(m->src);
    writeWord(m->dst);
    writeShort(m->amount);
    writeWord(m->amtSrc);
    writeWord(m->transform);
}

//---------------------------------------------------------
//   writeGen
//---------------------------------------------------------

void SoundFont::writeGen(const char* fourcc, QList<Zone*>* zones)
{
    write(fourcc, 4);
    int n = 0;
    foreach(const Zone* z, *zones)
        n += z->generators.size();
    writeDword((n + 1) * 4);

    foreach(const Zone* zone, *zones) {
        foreach(const GeneratorList* g, zone->generators)
            writeGenerator(g);
    }
    GeneratorList gen;
    memset(&gen, 0, sizeof(gen));
    writeGenerator(&gen);
}

//---------------------------------------------------------
//   writeGenerator
//---------------------------------------------------------

void SoundFont::writeGenerator(const GeneratorList* g)
{
    writeWord(g->gen);
    if (g->gen == Gen_KeyRange || g->gen == Gen_VelRange) {
        writeByte(g->amount.lo);
        writeByte(g->amount.hi);
    }
    else if (g->gen == Gen_Instrument)
        writeWord(g->amount.uword);
    else
        writeWord(g->amount.sword);
}

//---------------------------------------------------------
//   writeInst
//---------------------------------------------------------

void SoundFont::writeInst()
{
    write("inst", 4);
    int n = _instruments.size();
    writeDword((n + 1) * 22);
    int zoneIdx = 0;
    foreach(const Instrument* p, _instruments) {
        writeInstrument(zoneIdx, p);
        zoneIdx += p->zones.size();
    }
    Instrument p;
    writeInstrument(zoneIdx, &p);
}

//---------------------------------------------------------
//   writeInstrument
//---------------------------------------------------------

void SoundFont::writeInstrument(int zoneIdx, const Instrument* instrument)
{
    char name[20];
    memset(name, 0, 20);
    if (instrument->name)
        memcpy(name, instrument->name, strlen(instrument->name));
    write(name, 20);
    writeWord(zoneIdx);
}

//---------------------------------------------------------
//   writeShdr
//---------------------------------------------------------

void SoundFont::writeShdr()
{
    write("shdr", 4);
    writeDword(46 * (_samples.size() + 1));
    foreach(const Sample* s, _samples)
        writeSample(s);
    Sample s;
    memset(&s, 0, sizeof(s));
    writeSample(&s);
}

//---------------------------------------------------------
//   writeSample
//---------------------------------------------------------

void SoundFont::writeSample(const Sample* s)
{
    char name[20];
    memset(name, 0, 20);
    if (s->name)
        memcpy(name, s->name, strlen(s->name));
    write(name, 20);
    writeDword(s->start);
    writeDword(s->end);
    writeDword(s->loopstart);
    writeDword(s->loopend);
    writeDword(s->samplerate);
    writeByte(s->origpitch);
    writeChar(s->pitchadj);
    writeWord(s->sampleLink);
    writeWord(s->sampletype);
}

//---------------------------------------------------------
//   writeCompressedSample
//---------------------------------------------------------

int SoundFont::writeCompressedSample(Sample* s, int quality)
{
    QFile f(_path);
    if (!f.open(QIODevice::ReadOnly)) {
        fprintf(stderr, "cannot open <%s>\n", qPrintable(f.fileName()));
        return 0;
    }
    f.seek(_samplePos + s->start * sizeof(short));
    int samples = s->end - s->start;

    short * ibuffer = new short[samples];
    f.read((char*)ibuffer, samples * sizeof(short));
    f.close();

    ogg_stream_state os;
    ogg_page         og;
    ogg_packet       op;
    vorbis_info      vi;
    vorbis_dsp_state vd;
    vorbis_block     vb;
    vorbis_comment   vc;

    vorbis_info_init(&vi);

    float qualityF = 1.0f;
    switch (quality) {
    case 0: qualityF = 0.2f; break; // Low quality
    case 1: qualityF = 0.6f; break; // Medium quality
    case 2: qualityF = 1.0f; break; // High quality
    }

    int ret = vorbis_encode_init_vbr(&vi, 1, s->samplerate, qualityF);
    if (ret) {
        fprintf(stderr, "vorbis init failed\n");
        return false;
    }

    vorbis_comment_init(&vc);
    vorbis_analysis_init(&vd, &vi);
    vorbis_block_init(&vd, &vb);
    srand(time(NULL));
    ogg_stream_init(&os, rand());

    ogg_packet header;
    ogg_packet header_comm;
    ogg_packet header_code;

    vorbis_analysis_headerout(&vd, &vc, &header, &header_comm, &header_code);
    ogg_stream_packetin(&os, &header);
    ogg_stream_packetin(&os, &header_comm);
    ogg_stream_packetin(&os, &header_code);

    char* obuf = new char[1048576]; // 1024 * 1024
    char* p = obuf;

    for (;;) {
        int result = ogg_stream_flush(&os, &og);
        if (result == 0)
            break;
        memcpy(p, og.header, og.header_len);
        p += og.header_len;
        memcpy(p, og.body, og.body_len);
        p += og.body_len;
    }

    long i;
    int page = 0;

    for(;;) {
        int bufflength = qMin(BLOCK_SIZE, samples-page*BLOCK_SIZE);
        float **buffer = vorbis_analysis_buffer(&vd, bufflength);
        int j = 0;
        int max = qMin((page+1)*BLOCK_SIZE, samples);
        for (i = page * BLOCK_SIZE; i < max ; i++) {
            buffer[0][j] = ibuffer[i] / 32768.f;
            // buffer[0][j] = ibuffer[i] / 35000.f; // HACK: attenuate samples due to libsndfile bug
            j++;
        }

        vorbis_analysis_wrote(&vd, bufflength);

        while (vorbis_analysis_blockout(&vd, &vb) == 1) {
            vorbis_analysis(&vb, 0);
            vorbis_bitrate_addblock(&vb);

            while (vorbis_bitrate_flushpacket(&vd, &op)) {
                ogg_stream_packetin(&os, &op);

                for(;;) {
                    int result = ogg_stream_pageout(&os, &og);
                    if (result == 0)
                        break;
                    memcpy(p, og.header, og.header_len);
                    p += og.header_len;
                    memcpy(p, og.body, og.body_len);
                    p += og.body_len;
                }
            }
        }
        page++;
        if ((max == samples) || !((samples-page*BLOCK_SIZE)>0))
            break;
    }

    vorbis_analysis_wrote(&vd, 0);

    while (vorbis_analysis_blockout(&vd, &vb) == 1) {
        vorbis_analysis(&vb, 0);
        vorbis_bitrate_addblock(&vb);

        while (vorbis_bitrate_flushpacket(&vd, &op)) {
            ogg_stream_packetin(&os, &op);

            for(;;) {
                int result = ogg_stream_pageout(&os, &og);
                if (result == 0)
                    break;
                memcpy(p, og.header, og.header_len);
                p += og.header_len;
                memcpy(p, og.body, og.body_len);
                p += og.body_len;
            }
        }
    }

    delete [] ibuffer;

    ogg_stream_clear(&os);
    vorbis_block_clear(&vb);
    vorbis_dsp_clear(&vd);
    vorbis_comment_clear(&vc);
    vorbis_info_clear(&vi);

    int n = p - obuf;
    write(obuf, n);
    delete [] obuf;
    return n;
}

//---------------------------------------------------------
//   writeCSample
//---------------------------------------------------------

bool SoundFont::writeCSample(Sample* s, int idx)
{
    QFile fi(_path);
    if (!fi.open(QIODevice::ReadOnly)) {
        fprintf(stderr, "cannot open <%s>\n", qPrintable(fi.fileName()));
        return false;
    }
    fi.seek(_samplePos + s->start * sizeof(short));
    int samples = s->end - s->start;
    //short ibuffer[samples];
    short * ibuffer = new short[samples];
    fi.read((char*)ibuffer, samples * sizeof(short));
    fi.close();

    fprintf(_f, "static const short wave%d[] = {\n      ", idx);
    int n = 0;
    for (int row = 0;; ++row) {
        for (int col = 0; col < 16; ++col) {
            if (n >= samples)
                break;
            if (col != 0)
                fprintf(_f, ", ");
            fprintf(_f, "%d", ibuffer[n]);
            ++n;
        }
        if (n >= samples)
            break;
        fprintf(_f, ",\n      ");
    }
    delete [] ibuffer;
    fprintf(_f, "\n      };\n");
    return true;
}
