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

#ifndef CONVERSION_DLS_H
#define CONVERSION_DLS_H

#include <QObject>
#include "basetypes.h"
#include "dls/dlsdefinitions.h"
class SoundfontManager;
class QFile;

// Conversion of a soundfont into a DLS level 2 file
// Each preset is an instrument whose regions are the combinations of the preset and instrument divisions
class ConversionDls : public QObject
{
    Q_OBJECT

public:
    ConversionDls();

    /// Convert a soundfont, return an error if any
    QString convert(QString fileName, int sf2Index);

private:
    // Part of a sample
    struct Wave
    {
        int sampleId;
        quint32 start;
        quint32 end;
    };

    struct Region
    {
        quint8 keyMin;
        quint8 keyMax;
        quint8 velMin;
        quint8 velMax;
        int exclusiveClass;
        quint16 keyGroup;
        quint16 layer;
        int sampleId;
        int waveIndex;
        quint16 unityNote;
        qint16 fineTune;
        qint32 gain;
        bool hasLoop;
        quint32 loopType;
        quint32 loopStart;
        quint32 loopLength;
        quint16 linkOptions;
        quint16 phaseGroup;
        quint32 channel;
        bool hasArticulation;
        QList<Dls::Connection> connections;
    };

    struct Instrument
    {
        QString name;
        quint32 bank;
        quint32 program;
        QList<Region> regions;
        QList<Dls::Connection> connections;
    };

    // Build the instruments
    Instrument getInstrument(int presetIndex);
    void addRegions(EltID idPrstInst, Instrument &instrument, quint16 layer);
    bool getRegion(EltID idPrstInst, EltID idInstSmpl, Region &region);
    QList<ModulatorData> getModulators(EltID idPrstInst, EltID idInstSmpl);
    QList<ModulatorData> readModulators(EltID id);
    static void overrideModulators(QList<ModulatorData> &modulators, const QList<ModulatorData> &newModulators);
    QList<Dls::Connection> getConnections(const QVector<int> &generators, const QList<ModulatorData> &modulators);
    void addModulatorConnection(QList<Dls::Connection> &connections, const ModulatorData &modulator);
    void linkStereoRegions(Instrument &instrument);
    void moveCommonArticulation(Instrument &instrument);
    int getWaveIndex(int sampleId, quint32 start, quint32 end);

    // Write the file
    void writeInstrument(QFile &file, const Instrument &instrument);
    void writeWave(QFile &file, const Wave &wave);
    void writeInfo(QFile &file, const QMap<QByteArray, QString> &info);
    static QByteArray getWaveSampleData(quint16 unityNote, qint16 fineTune, qint32 gain, bool hasLoop,
                                        quint32 loopType, quint32 loopStart, quint32 loopLength);
    static QByteArray getArticulationData(const QList<Dls::Connection> &connections);

    // Generic functions for RIFF files
    void beginList(QFile &file, const char * id, const char * type);
    void endList(QFile &file);
    static void writeChunk(QFile &file, const char * id, const QByteArray &data);

    // Utils
    static void addConnection(QList<Dls::Connection> &connections, quint16 source, quint16 control, quint16 destination,
                              quint16 transform, double value);
    static bool getDlsSource(SFModulator source, quint16 &dlsSource, quint16 &transform, bool &isBipolar, bool &isInverted);
    static bool getDlsDestination(quint16 champ, quint16 &internalSource, quint16 &destination, double &factor);
    // Connection implied by the DLS format, possibly with its default value
    static bool isImpliedConnection(const Dls::Connection &connection, bool withDefaultValue);
    static bool isEqual(const QList<Dls::Connection> &list1, const QList<Dls::Connection> &list2);
    static void appendUInt16(QByteArray &data, quint16 value);
    static void appendUInt32(QByteArray &data, quint32 value);

    SoundfontManager * _sm;
    int _sf2Index;
    QList<Wave> _waves;
    QList<qint64> _listPositions;
};

#endif // CONVERSION_DLS_H
