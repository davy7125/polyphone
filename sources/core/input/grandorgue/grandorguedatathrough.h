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

#ifndef GRANDORGUEDATATHROUGH_H
#define GRANDORGUEDATATHROUGH_H

#include <QMap>
class GrandOrgueRank;
class GrandOrgueStop;

class GrandOrgueDataThrough
{
public:
    GrandOrgueDataThrough();

    // Gain per rank (instrument)
    void setMaxRankGain(int rankId, double gain);
    double getMaxRankGain(int rankId);

    // Finalize the pre-process
    void finalizePreprocess();

    // Maximum gain found in the sample set
    double getMaxGain() { return _maxGain; }

    // Match between a sample name and a sample index / loop mode (stereo samples have 2 ids)
    void setSf2SmplId(QString filePath, QList<int> sf2ElementIds, bool hasLoop);
    QList<int> getSf2SmplId(QString filePath);
    bool hasLoop(QString filePath);

    // Store all created sample name and check if a name already exists
    void storeSampleName(QString sampleName);
    bool sampleNameExists(QString sampleName);

    void useNextBank();
    void getNextBankPreset(int &bank, int &preset);

private:
    QMap<int, double> _maxGainPerRank;
    double _maxGain;
    QMap<QString, QList<int> > _smplIds;
    QMap<QString, bool> _hasLoop;
    QList<QString> _sampleNames;
    int _currentBank;
    int _currentPreset;
};

#endif // GRANDORGUEDATATHROUGH_H
