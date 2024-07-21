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

#ifndef RUNNABLECHORDCREATOR_H
#define RUNNABLECHORDCREATOR_H

#include <QRunnable>
#include "basetypes.h"
#include "toolchords_parameters.h"
class ToolChords;

class RunnableChordCreator: public QRunnable
{
public:
    RunnableChordCreator(ToolChords * tool, EltID idInst, ChordInfo ci, quint32 key, quint32 minKey, bool loop, bool stereo, int side);
    ~RunnableChordCreator() override;
    void run() override;

private:
    EltID closestSample(EltID idInst, double pitch, double &ecart, int cote, EltID &idInstSmpl);
    QVector<float> getSampleData(EltID idSmpl, quint32 nbRead);
    void addSampleData(QVector<float> &vData1, QVector<float> &vData2, double mult);
    static QMap<int, int> getChordKeys(quint32 key, ChordInfo& chordInfo);

    static double SAMPLE_DURATION; // In seconds
    static quint32 SAMPLE_RATE; // In samples per second

    ToolChords * _tool;
    EltID _idInst;
    ChordInfo _ci;
    quint32 _key, _minKey;
    bool _loop;
    bool _stereo;
    int _side;
};

#endif // RUNNABLECHORDCREATOR_H
