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

#ifndef SFZPARAMETERREGION_H
#define SFZPARAMETERREGION_H

#include "sfzparameter.h"
#include "basetypes.h"
class SoundfontManager;

class SfzParameterRegion
{
public:
    // Load
    SfzParameterRegion() {}
    void operator << (SfzParameter param) { _listeParam << param; }
    void mergeIfNotDefined(SfzParameterRegion &groupToMerge);

    // Decode
    void decode(SoundfontManager * sf2, EltID idElt) const;
    QList<int> getSampleIndex(SoundfontManager * sf2, EltID idElt, QString pathSfz) const;
    void adaptOffsets(int startLoop, int endLoop, int length);
    void adjustStereoVolumeAndCorrection(QString path, int defaultCorrection);
    bool sampleValid(QString path);
    void checkFilter();
    void checkKeyTrackedFilter(bool remove);
    void adjustVolume(double offset);
    void adjustCorrection(int offset, int defaultCorrection);
    void removeOpCode(SfzParameter::OpCode opcode)
    {
        for (int i = _listeParam.size() - 1; i >= 0; i--)
            if (_listeParam.at(i).getOpCode() == opcode)
                _listeParam.removeAt(i);
    }

    // Lecture
    bool isDefined(SfzParameter::OpCode opcode) const
    {
        bool bRet = false;
        for (int i = 0; i < _listeParam.size(); i++)
            if (_listeParam.at(i).getOpCode() == opcode)
                bRet = true;
        return bRet;
    }
    int getIntValue(SfzParameter::OpCode opcode) const
    {
        int iRet = 0;
        for (int i = 0; i < _listeParam.size(); i++)
            if (_listeParam.at(i).getOpCode() == opcode)
                iRet = _listeParam.at(i).getIntValue();
        return iRet;
    }
    double getDoubleValue(SfzParameter::OpCode opcode) const
    {
        double dRet = 0;
        for (int i = 0; i < _listeParam.size(); i++)
            if (_listeParam.at(i).getOpCode() == opcode)
                dRet = _listeParam.at(i).getDoubleValue();
        return dRet;
    }
    QString getStrValue(SfzParameter::OpCode opcode) const
    {
        QString strRet = "";
        for (int i = 0; i < _listeParam.size(); i++)
            if (_listeParam.at(i).getOpCode() == opcode)
                strRet = _listeParam.at(i).getStringValue();
        return strRet;
    }
    static double percentToDB(double percent) { return log10(percent / 100.) * 20.; }

private:
    QList<SfzParameter> _listeParam;
    static QStringList getFullPath(QString base, QStringList directories);
    static double log2m1200(double value) { return 1200. * qLn(qMax(0.001, value)) / 0.69314718056 /* ln(2) */; }
    static double d1200e2(int value) { return qPow(2., static_cast<double>(value) / 1200.); }
    static void addSeconds(double value, AttributeType champ, SoundfontManager * sf2, EltID id);
    void getKeynumValues(double &baseValue, int &keynum, SfzParameter::OpCode opCodeKeynum, SfzParameter::OpCode opCodeBase) const;
    static QString getName(QString name, int maxCharacters, int suffixNumber, QString suffix = "");
};

#endif // SFZPARAMETERREGION_H
