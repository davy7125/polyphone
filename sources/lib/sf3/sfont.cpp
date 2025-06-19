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

#include <stdio.h>
//#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <vorbis/vorbisenc.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

#include "sfont.h"
#include "time.h"

#ifndef SFTOOLS_NOXML
#include "xml.h"
#include <sndfile.h>
#endif

#include <QtCore/QFile>

using namespace SfTools;

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

//---------------------------------------------------------
//   Sample
//---------------------------------------------------------

Sample::Sample()
{
    name = nullptr;
    start = 0;
    end = 0;
    loopstart = 0;
    loopend = 0;
    samplerate = 0;
    origpitch = 0;
    pitchadj = 0;
    sampleLink = 0;
    sampletype = 0;
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
    name = nullptr;
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
    path      = s;
    engine    = nullptr;
    name      = nullptr;
    date      = nullptr;
    comment   = nullptr;
    tools     = nullptr;
    creator   = nullptr;
    product   = nullptr;
    copyright = nullptr;
    irom      = nullptr;
    version.major = 0;
    version.minor = 0;
    iver.major = 0;
    iver.minor = 0;
    _smallSf   = false;
}

SoundFont::~SoundFont()
{
    free(engine);
    free(name);
    free(date);
    free(comment);
    free(tools);
    free(creator);
    free(product);
    free(copyright);
    free(irom);
}

//---------------------------------------------------------
//   read
//---------------------------------------------------------

bool SoundFont::read()
{
    file = new QFile(path);
    if (!file->open(QIODevice::ReadOnly)) {
        fprintf(stderr, "cannot open <%s>\n", qPrintable(path));
        delete file;
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
        printf("read sf file failed: %s\n", qPrintable(s));
        delete file;
        return false;
    }
    delete file;
    return true;
}

//---------------------------------------------------------
//   skip
//---------------------------------------------------------

void SoundFont::skip(int n)
{
    qint64 pos = file->pos();
    if (!file->seek(pos + n))
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
    if (file->read(signature, 4) != 4)
        throw(QString("unexpected end of file\n"));
}

//---------------------------------------------------------
//   readDword
//---------------------------------------------------------

unsigned SoundFont::readDword()
{
    unsigned format;
    if (file->read((char*)&format, 4) != 4)
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
    if (file->read((char*)&format, 2) != 2)
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
    if (file->read((char*)&format, 2) != 2)
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
    if (file->read((char*)&val, 1) != 1)
        throw(QString("unexpected end of file\n"));
    return val;
}

//---------------------------------------------------------
//   readChar
//---------------------------------------------------------

int SoundFont::readChar()
{
    char val;
    if (file->read(&val, 1) != 1)
        throw(QString("unexpected end of file\n"));
    return val;
}

//---------------------------------------------------------
//   readVersion
//---------------------------------------------------------

void SoundFont::readVersion(sfVersionTag * v)
{
    unsigned char data[4];
    if (file->read((char*)data, 4) != 4)
        throw(QString("unexpected end of file\n"));
    v->major = data[0] + (data[1] << 8);
    v->minor = data[2] + (data[3] << 8);
}

//---------------------------------------------------------
//   readString
//---------------------------------------------------------

char* SoundFont::readString(int n)
{
    char data[2500];
    int skip = 0;
    if (n > 2500)
    {
        skip = n - 2500;
        n = 2500;
    }
    if (file->read((char*)data, n) != n)
        throw(QString("unexpected end of file\n"));
    if (skip > 0)
    {
        if (file->skip(skip) != skip)
            throw(QString("unexpected end of file\n"));
    }
    if (data[n-1] != 0)
        data[n] = 0;
    return strdup(data);
}

//---------------------------------------------------------
//   readSection
//---------------------------------------------------------

void SoundFont::readSection(const char* fourcc, int len)
{
    //printf("readSection <%s> len %d\n", fourcc, len);

    switch(FOURCC(fourcc[0], fourcc[1], fourcc[2], fourcc[3])) {
    case FOURCC('i', 'f', 'i', 'l'):    // version
        readVersion(&version);
        break;
    case FOURCC('I','N','A','M'):       // sound font name
        name = readString(len);
        break;
    case FOURCC('i','s','n','g'):       // target render engine
        engine = readString(len);
        break;
    case FOURCC('I','P','R','D'):       // product for which the bank was intended
        product = readString(len);
        break;
    case FOURCC('I','E','N','G'): // sound designers and engineers for the bank
        creator = readString(len);
        break;
    case FOURCC('I','S','F','T'): // SoundFont tools used to create and alter the bank
        tools = readString(len);
        break;
    case FOURCC('I','C','R','D'): // date of creation of the bank
        date = readString(len);
        break;
    case FOURCC('I','C','M','T'): // comments on the bank
        comment = readString(len);
        break;
    case FOURCC('I','C','O','P'): // copyright message
        copyright = readString(len);
        break;
    case FOURCC('s','m','p','l'): // the digital audio samples
        samplePos = file->pos();
        sampleLen = len;
        skip(len);
        break;
    case FOURCC('s','m','2','4'): // audio samples (24-bit part)
        skip(len); // Just skip it
        break;
    case FOURCC('p','h','d','r'): // preset headers
        readPhdr(len);
        break;
    case FOURCC('p','b','a','g'): // preset index list
        readBag(len, &pZones);
        break;
    case FOURCC('p','m','o','d'): // preset modulator list
        readMod(len, &pZones);
        break;
    case FOURCC('p','g','e','n'): // preset generator list
        readGen(len, &pZones);
        break;
    case FOURCC('i','n','s','t'): // instrument names and indices
        readInst(len);
        break;
    case FOURCC('i','b','a','g'): // instrument index list
        readBag(len, &iZones);
        break;
    case FOURCC('i','m','o','d'): // instrument modulator list
        readMod(len, &iZones);
        break;
    case FOURCC('i','g','e','n'): // instrument generator list
        readGen(len, &iZones);
        break;
    case FOURCC('s','h','d','r'): // sample headers
        readShdr(len);
        break;
    case FOURCC('i', 'r', 'o', 'm'):    // sample rom
        irom = readString(len);
        break;
    case FOURCC('i', 'v', 'e', 'r'):    // sample rom version
        readVersion(&iver);
        break;
    default:
        skip(len);
        //throw(QString("unknown fourcc <%1>").arg(fourcc));
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
        printf("no presets\n");
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
                presets.back()->zones.append(z);
                pZones.append(z);
            }
        }
        index1 = index2;
        presets.append(preset);
    }
    presets.takeLast();
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
                instruments.back()->zones.append(z);
                iZones.append(z);
            }
        }
        index1 = index2;
        instruments.append(instrument);
    }
    instruments.takeLast();
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

        // printf("readFontHeader %d %d   %d %d\n", s->start, s->end, s->loopstart, s->loopend);
        samples.append(s);
    }
    skip(46);   // trailing record
}

//---------------------------------------------------------
//   write
//---------------------------------------------------------

bool SoundFont::write()
{
    qint64 riffLenPos;
    qint64 listLenPos;
    try {
        file->write("RIFF", 4);
        riffLenPos = file->pos();
        writeDword(0);
        file->write("sfbk", 4);

        file->write("LIST", 4);
        listLenPos = file->pos();
        writeDword(0);
        file->write("INFO", 4);

        writeIfil();
        if (name)
            writeStringSection("INAM", name);
        if (engine)
            writeStringSection("isng", engine);
        if (product)
            writeStringSection("IPRD", product);
        if (creator)
            writeStringSection("IENG", creator);
        if (tools)
            writeStringSection("ISFT", tools);
        if (date)
            writeStringSection("ICRD", date);
        if (comment)
            writeStringSection("ICMT", comment);
        if (copyright)
            writeStringSection("ICOP", copyright);
        if (irom)
            writeStringSection("irom", irom);
        writeIver();

        qint64 pos = file->pos();
        file->seek(listLenPos);
        writeDword(pos - listLenPos - 4);
        file->seek(pos);

        file->write("LIST", 4);
        listLenPos = file->pos();
        writeDword(0);
        file->write("sdta", 4);
        writeSmpl();
        pos = file->pos();
        file->seek(listLenPos);
        writeDword(pos - listLenPos - 4);
        file->seek(pos);

        file->write("LIST", 4);
        listLenPos = file->pos();
        writeDword(0);
        file->write("pdta", 4);

        writePhdr();
        writeBag("pbag", &pZones);
        writeMod("pmod", &pZones);
        writeGen("pgen", &pZones);
        writeInst();
        writeBag("ibag", &iZones);
        writeMod("imod", &iZones);
        writeGen("igen", &iZones);
        writeShdr();

        pos = file->pos();
        file->seek(listLenPos);
        writeDword(pos - listLenPos - 4);
        file->seek(pos);

        qint64 endPos = file->pos();
        file->seek(riffLenPos);
        writeDword(endPos - riffLenPos - 4);
    }
    catch (QString s) {
        printf("write sf file failed: %s\n", qPrintable(s));
        return false;
    }
    return true;
}

bool SoundFont::compress(QFile* f, double oggQuality, qint64 oggSerial)
{
    file = f;
    _compress = true;
    _oggQuality = oggQuality;
    _oggSerial = oggSerial;

    return write();
}

bool SoundFont::uncompress(QFile* f)
{
    file = f;
    _compress = false;
    return write();
}

//---------------------------------------------------------
//   write
//---------------------------------------------------------

void SoundFont::write(const char* p, int n)
{
    if (file->write(p, n) != n)
        throw(QString("write error"));
}

//---------------------------------------------------------
//   writeStringSection
//---------------------------------------------------------

void SoundFont::writeStringSection(const char* fourcc, char* s)
{
    write(fourcc, 4);
    int nn = strlen(s) + 1;
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
    if (_compress) {
        version.major = 3;
        version.minor = 0;
    } else {
        version.major = 2;
        version.minor = 1;
    }
    data[0] = version.major;
    data[1] = version.major >> 8;
    data[2] = version.minor;
    data[3] = version.minor >> 8;
    write((char*)data, 4);
}

//---------------------------------------------------------
//   writeIVer
//---------------------------------------------------------

void SoundFont::writeIver()
{
    write("iver", 4);
    writeDword(4);
    unsigned char data[4];
    data[0] = iver.major;
    data[1] = iver.major >> 8;
    data[2] = iver.minor;
    data[3] = iver.minor >> 8;
    write((char*)data, 4);
}

//---------------------------------------------------------
//   writeSmpl
//---------------------------------------------------------

void SoundFont::writeSmpl()
{
    write("smpl", 4);

    qint64 pos = file->pos();
    writeDword(0);
    int currentSamplePos = 0;
    if (_compress)
    {
        // Compress wave data
        foreach(Sample* s, samples)
        {
            // Loop start and end are now based on the beginning of each sample
            s->loopstart -= s->start;
            s->loopend -= s->start;

            // OGG flag added
            s->sampletype |= 0x10;
            int len  = writeCompressedSample(s); // In byte
            s->start = currentSamplePos;
            currentSamplePos += len;
            s->end = currentSamplePos;

            // Possible padding
            if (len % 4 != 0)
            {
                int padNumber = 4 - (len % 4);
                for (int i = 0; i < padNumber; i++)
                    write("\0", 1);
                currentSamplePos += padNumber;
            }
        }
    }
    else
    {
        // Uncompress from OGG data
        foreach(Sample* s, samples)
        {
            // OGG flag is removed
            int len = 0;
            if (s->sampletype & 0x10)
            {
                s->sampletype &= ~0x10;
                len = writeUncompressedSample(s) / 2; // In sample data points (16 bits)
            }
            else
            {
                // The sample is not compressed in the sf3 file

                // Prepare input data
                QFile f(path);
                if (f.open(QIODevice::ReadOnly))
                {
                    f.seek(samplePos + s->start * sizeof(short));
                    len = s->end - s->start;
                    short * ibuffer = new short[len];
                    f.read((char*)ibuffer, len * sizeof(short));
                    f.close();

                    write((char *)ibuffer, len * sizeof(short));
                }
                else
                    fprintf(stderr, "cannot open <%s>\n", qPrintable(f.fileName()));
            }

            s->start = currentSamplePos;
            currentSamplePos += len;
            s->end = currentSamplePos;

            // Loop start and end are now based on the beginning of the section "smpl"
            s->loopstart += s->start;
            s->loopend   += s->start;
        }
    }
    qint64 npos = file->pos();
    file->seek(pos);
    writeDword(npos - pos - 4);
    file->seek(npos);
}

//---------------------------------------------------------
//   writePhdr
//---------------------------------------------------------

void SoundFont::writePhdr()
{
    write("phdr", 4);
    int n = presets.size();
    writeDword((n + 1) * 38);
    int zoneIdx = 0;
    foreach(const Preset* p, presets) {
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
    int n = instruments.size();
    writeDword((n + 1) * 22);
    int zoneIdx = 0;
    foreach(const Instrument* p, instruments) {
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
    writeDword(46 * (samples.size() + 1));
    foreach(const Sample* s, samples)
        writeSample(s);
    Sample s;
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

int SoundFont::writeCompressedSample(Sample* s)
{
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly)) {
        fprintf(stderr, "cannot open <%s>\n", qPrintable(f.fileName()));
        return 0;
    }
    f.seek(samplePos + s->start * sizeof(short));
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
    int ret = vorbis_encode_init_vbr(&vi, 1, s->samplerate, _oggQuality);
    if (ret) {
        printf("vorbis init failed\n");
        delete [] ibuffer;
        return false;
    }
    vorbis_comment_init(&vc);
    vorbis_analysis_init(&vd, &vi);
    vorbis_block_init(&vd, &vb);
    srand(time(nullptr));
    ogg_stream_init(&os, _oggSerial == std::numeric_limits<qint64>::max() ? rand() : (int)_oggSerial);

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
        int bufflength = qMin(BLOCK_SIZE, samples-page * BLOCK_SIZE);
        float **buffer = vorbis_analysis_buffer(&vd, bufflength);

        // Convert int16 to float
        int j = 0;
        float coef = 1.0f / ((0.5f + 0x7fff) * 1.009f); // 1.009 seems to be necessary, but why?
        int max = qMin((page + 1) * BLOCK_SIZE, samples);
        for (i = page * BLOCK_SIZE; i < max ; i++) {
            buffer[0][j] = (0.5f + ibuffer[i]) * coef;
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

    ogg_stream_clear(&os);
    vorbis_block_clear(&vb);
    vorbis_dsp_clear(&vd);
    vorbis_comment_clear(&vc);
    vorbis_info_clear(&vi);

    int n = p - obuf;
    write(obuf, n);

    delete [] obuf;
    delete [] ibuffer;
    return n;
}

//---------------------------------------------------------
//   writeCSample
//---------------------------------------------------------

bool SoundFont::writeCSample(Sample* s, int idx)
{
    QFile fi(path);
    if (!fi.open(QIODevice::ReadOnly)) {
        fprintf(stderr, "cannot open <%s>\n", qPrintable(fi.fileName()));
        return false;
    }
    fi.seek(samplePos + s->start * sizeof(short));
    int samples = s->end - s->start;
    short * ibuffer = new short[samples];
    fi.read((char*)ibuffer, samples * sizeof(short));
    fi.close();

    fprintf(f, "const short wave%d[] = {\n      ", idx);
    int n = 0;
    for (int row = 0;; ++row) {
        for (int col = 0; col < 16; ++col) {
            if (n >= samples)
                break;
            if (col != 0)
                fprintf(f, ", ");
            fprintf(f, "%d", ibuffer[n]);
            ++n;
        }
        if (n >= samples)
            break;
        fprintf(f, ",\n      ");
    }
    fprintf(f, "\n      };\n");
    delete [] ibuffer;
    return true;
}

static size_t ovRead(void* ptr, size_t size, size_t nmemb, void* datasource);
static int ovSeek(void* datasource, ogg_int64_t offset, int whence);
static long ovTell(void* datasource);
static ov_callbacks ovCallbacks = { ovRead, ovSeek, nullptr, ovTell };

//---------------------------------------------------------
//   ovRead
//---------------------------------------------------------

static size_t ovRead(void* ptr, size_t size, size_t nmemb, void* datasource)
{
    VorbisData* vd = (VorbisData*)datasource;
    size_t n = size * nmemb;
    if (vd->data.size() < int(vd->pos + n))
        n = vd->data.size() - vd->pos;
    if (n) {
        const char* src = vd->data.data() + vd->pos;
        memcpy(ptr, src, n);
        vd->pos += n;
    }

    return n;
}

//---------------------------------------------------------
//   ovSeek
//---------------------------------------------------------

static int ovSeek(void* datasource, ogg_int64_t offset, int whence)
{
    VorbisData* vd = (VorbisData*)datasource;
    switch(whence) {
    case SEEK_SET:
        vd->pos = offset;
        break;
    case SEEK_CUR:
        vd->pos += offset;
        break;
    case SEEK_END:
        vd->pos = vd->data.size() - offset;
        break;
    }
    return 0;
}

//---------------------------------------------------------
//   ovTell
//---------------------------------------------------------

static long ovTell(void* datasource)
{
    VorbisData* vd = (VorbisData*)datasource;
    return vd->pos;
}

//---------------------------------------------------------
//   writeUncompressedSample
//---------------------------------------------------------

int SoundFont::writeUncompressedSample(Sample* s)
{
    // Prepare input data
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly)) {
        fprintf(stderr, "cannot open <%s>\n", qPrintable(f.fileName()));
        return 0;
    }
    f.seek(samplePos + s->start);
    int oggSize = s->end - s->start;
    short * ibuffer = new short[oggSize];
    f.read((char*)ibuffer, oggSize);
    f.close();

    _vorbisData.pos  = 0;
    _vorbisData.data = QByteArray((char *)ibuffer, oggSize);

    // Decode
    QByteArray decodedData;
    OggVorbis_File vf;
    _vorbisData.pos  = 0;
    int length = 0;
    if (ov_open_callbacks(&_vorbisData, &vf, nullptr, 0, ovCallbacks) == 0)
    {
        short buffer[2048];
        int numberRead = 0;
        int section = 0;
        do {
            numberRead = ov_read(&vf, (char *)buffer, 2048 * sizeof(short), 0, 2, 1, &section);
            write((char *)buffer, numberRead);
            length += numberRead;
        } while (numberRead);

        ov_clear(&vf);
    }

    delete[] ibuffer;
    return length;
}
