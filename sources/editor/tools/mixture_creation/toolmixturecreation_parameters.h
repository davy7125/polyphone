/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2019 Davy Triponney                                **
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
**  Website/Contact: https://www.polyphone-soundfonts.com                 **
**             Date: 01.01.2013                                           **
***************************************************************************/

#ifndef TOOLMIXTURECREATION_PARAMETERS_H
#define TOOLMIXTURECREATION_PARAMETERS_H

#include "abstracttoolparameters.h"
#include <QList>
#include <QString>
#include "qmath.h"

class RankInfo
{
public:
    RankInfo(int overtoneType, int octave) :
        _overtoneType(overtoneType),
        _octave(octave)
    {}
    
    // 1 is fundamental or octave, 3 is for a fifth (frequency * 3), 5 is approximately a third (frequency * 5), ...
    int getOvertoneType() { return _overtoneType; }
    void setOvertoneType(int overtoneType) { _overtoneType = overtoneType; }
  
    // 0 is based on the fundamental, 1 one octave higher, -1 one octave lower, ...
    int getOctave() { return _octave; }
    void setOctave(int octave) { _octave = octave; }

    double getOffset()
    {
        // Calcul du multiple de la fréquence fondamentale
        double multiple = (double)(2 * _overtoneType + 1) * pow(2.0f, _octave - 3);

        // Renvoi du nombre de demi-tons à ajouter à la fondamentale pour obtenir l'harmonique
        return 12. * qLn(multiple) / 0.69314718056;
    }
    
private:
    int _overtoneType;
    int _octave;
};

class DivisionInfo
{
public:
    DivisionInfo(int minKey, int maxKey) :
        _minKey(minKey),
        _maxKey(maxKey)
    {}
    
    // Start of the division
    int getMinKey() { return _minKey; }
    void setMinKey(int minKey) { _minKey = minKey; }
    
    // End of the division
    int getMaxKey() { return _maxKey; }
    void setMaxKey(int maxKey) { _maxKey = maxKey; }
    
    // Add / remove a rank
    void addRank(int overtoneType, int octave)
    {
        _ranks << RankInfo(overtoneType, octave);
    }
    void removeRank(int number)
    {
        _ranks.removeAt(number);
    }
    
    // Get all ranks
    QList<RankInfo>& getRanks() { return _ranks; }
    
private:
    int _minKey;
    int _maxKey;
    QList<RankInfo> _ranks;
};

class ToolMixtureCreation_parameters: public AbstractToolParameters
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
    
    QList<DivisionInfo> getDivisions() { return _divisions; }
    void setDivisions(QList<DivisionInfo> divisions) { _divisions = divisions; }
    
private:
    QString _instrumentName;
    int _densityType;
    bool _stereoSample;
    bool _loopSample;
    QList<DivisionInfo> _divisions;
};

#endif // TOOLMIXTURECREATION_PARAMETERS_H
