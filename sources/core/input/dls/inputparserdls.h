/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2024 Davy Triponney                                **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program. If not, see http://www.gnu.org/licenses/.    **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: https://www.polyphone.io                             **
**             Date: 01.01.2013                                           **
***************************************************************************/

#ifndef INPUTPARSERDLS_H
#define INPUTPARSERDLS_H

#include "abstractinputparser.h"
#include "basetypes.h"
#include "dlsdefinitions.h"
class SoundfontManager;
class QFile;

class InputParserDls : public AbstractInputParser
{
    Q_OBJECT

public:
    InputParserDls();
    bool canFastLoad() override { return true; }

protected slots:
    void processInternal(QString fileName, SoundfontManager * sm, bool &success, QString &error, int &sf2Index, QString &tempFilePath) override;

private:
    struct Chunk
    {
        QByteArray id;
        QByteArray listType; // For the chunks "RIFF" and "LIST"
        qint64 dataPosition;
        quint32 size;
    };

    struct WaveSample
    {
        bool isDefined = false;
        quint16 unityNote = 60;
        qint16 fineTune = 0;
        qint32 gain = 0;
        bool hasLoop = false;
        quint32 loopType = 0;
        quint32 loopStart = 0;
        quint32 loopLength = 0;
    };

    struct Wave
    {
        quint32 listPosition = 0; // Position of the chunk "LIST wave" in the file
        quint32 poolOffset = 0; // Offset referenced by the pool table
        QString name;
        quint16 formatTag = 0;
        quint16 channels = 0;
        quint32 sampleRate = 0;
        quint16 bitsPerSample = 0;
        quint16 blockAlign = 0;
        quint32 dataSize = 0;
        bool hasFact = false;
        quint32 factLength = 0;
        WaveSample waveSample;

        // Created samples (one per channel) and their properties
        QList<int> sampleIds;
        quint32 length = 0;
        int rootKey = 60;
        int correction = 0;
        quint32 loopStart = 0;
        quint32 loopEnd = 0;
    };

    struct Region
    {
        quint16 keyMin = 0;
        quint16 keyMax = 127;
        quint16 velMin = 0;
        quint16 velMax = 127;
        quint16 keyGroup = 0;
        WaveSample waveSample;
        bool hasWaveLink = false;
        quint16 linkOptions = 0;
        quint16 phaseGroup = 0;
        quint32 channel = Dls::WAVELINK_CHANNEL_LEFT;
        quint32 tableIndex = 0;
        bool hasArticulation = false;
        QList<Dls::Connection> connections;
    };

    struct Instrument
    {
        QString name;
        quint32 bank = 0;
        quint32 program = 0;
        QList<Dls::Connection> connections;
        QList<Region> regions;
    };

    struct Zone
    {
        int sampleId;
        QMap<AttributeType, AttributeValue> generators;
        double attenuation; // Not limited yet
        QList<ModulatorData> modulators;
    };

    // Parse the file
    bool parseFile(QString &error);
    QList<Chunk> readChunks(qint64 start, qint64 end);
    QList<Chunk> readSubChunks(const Chunk &list);
    QByteArray readData(const Chunk &chunk);
    void parseInfo(const Chunk &list, QMap<QByteArray, QString> &info);
    void parseWavePool(const Chunk &list);
    void parseWave(const Chunk &list, quint32 poolOffset);
    void parseInstrument(const Chunk &list);
    void parseRegion(const Chunk &list, Instrument &instrument);
    bool parseArticulationList(const Chunk &list, QList<Dls::Connection> &connectionsLevel1, QList<Dls::Connection> &connectionsLevel2);
    void parseArticulation(const QByteArray &data, bool isLevel1, QList<Dls::Connection> &connections);
    WaveSample parseWaveSample(const QByteArray &data);
    bool isConditionTrue(const QByteArray &data);
    bool isListConditionTrue(const QList<Chunk> &subChunks);
    static quint32 queryDevice(quint32 queryId, bool &isSupported);

    // Create the soundfont
    void createSamples(int sf2Index);
    void linkStereoSamples(int sf2Index);
    QList<Zone> getZones(const Instrument &instrument, const Region &region);
    void createInstrument(int sf2Index, const Instrument &instrument, QList<Zone> &zones);
    int getWaveIndex(quint32 tableIndex);
    static void setOffset(Zone &zone, AttributeType fineChamp, AttributeType coarseChamp, qint64 offset);
    static void writeModulators(SoundfontManager * sm, EltID id, const QList<ModulatorData> &modulators, int &index);

    QFile * _file;
    QString _fileName;
    SoundfontManager * _sm;
    QMap<QByteArray, QString> _info;
    QList<quint32> _poolTable;
    QList<Wave> _waves;
    QList<Instrument> _instruments;
};

#endif // INPUTPARSERDLS_H
