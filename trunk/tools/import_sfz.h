/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013 Davy Triponney                                     **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: http://www.polyphone.fr/                             **
**             Date: 01.01.2013                                           **
***************************************************************************/

#ifndef IMPORT_SFZ_H
#define IMPORT_SFZ_H

#include <QList>
#include <QString>
#include "sf2_types.h"

class Pile_sf2;

class Parametre
{
public:
    enum OpCode
    {
        op_unknown,
        op_sample,
        op_key,
        op_keyMin,
        op_keyMax,
        op_velMin,
        op_velMax,
        op_attenuation,
        op_balance,
        op_loopMode,
        op_rootKey,
        op_tuningFine,
        op_tuningCoarse,
        op_tuningScale,
        op_filterFreq,
        op_filterQ,
        op_volEnvDelay,
        op_volEnvAttack,
        op_volEnvHold,
        op_volEnvSustain,
        op_volEnvRelease,
        op_noteToVolEnvHold,
        op_noteToVolEnvDecay,
        op_modEnvDelay,
        op_modEnvAttack,
        op_modEnvHold,
        op_modEnvDecay,
        op_modEnvSustain,
        op_modEnvRelease,
        op_modEnvToTon,
        op_modEnvToFilter,
        op_noteToModEnvHold,
        op_noteToModEnvDecay,
        op_modLFOdelay,
        op_modLFOfreq,
        op_modLFOtoTon,
        op_modLFOtoFilter,
        op_modLFOtoVolume,
        op_vibLFOdelay,
        op_vibLFOfreq,
        op_vibLFOtoTon,
        op_exclusiveClass,
        op_chorus,
        op_reverb,
        op_sampleLoopStart,
        op_sampleLoopEnd,
        op_sampleStart,
        op_sampleEnd
    };

    Parametre(QString opcode, QString valeur);
    OpCode  getOpCode()      const { return _opcode;   }
    int     getIntValue()    const { return _intValue; }
    int     getDoubleValue() const { return _dblValue; }
    QString getStringValue() const { return _strValue; }

private:
    OpCode  _opcode;
    int     _intValue;
    double  _dblValue;
    QString _strValue;

    int getNumNote(QString noteStr);
};

class GroupeParametres
{
public:
    // Chargement
    GroupeParametres() {}
    void operator << (Parametre param) { _listeParam << param; }

    // Décodage
    void decode(Pile_sf2 * sf2, EltID idElt) const;
    bool containsSample() const
    {
        bool bRet = false;
        for (int i = 0; i < _listeParam.size(); i++)
            bRet = bRet || _listeParam.at(i).getOpCode() == Parametre::op_sample;
        return bRet;
    }

private:
    QList<Parametre> _listeParam;
};

class EnsembleGroupes
{
public:
    // Chargement
    EnsembleGroupes() {}
    void newGroup() { _listeDivisions << GroupeParametres(); }
    void addParam(QString opcode, QString valeur)
    {
        if (_listeDivisions.size() == 0)
            _paramGlobaux << Parametre(opcode, valeur);
        else
            _listeDivisions.last() << Parametre(opcode, valeur);
    }

    // Décodage
    void decode(Pile_sf2 * sf2, EltID idInst) const;

private:
    GroupeParametres _paramGlobaux;
    QList<GroupeParametres> _listeDivisions;
};


class ImportSfz
{
public:
    ImportSfz(Pile_sf2 *sf2);
    void import(QString fileName, int &numSf2);

private:
    enum Bloc
    {
        BLOC_UNKNOWN,
        BLOC_GROUP,
        BLOC_REGION
    };

    Pile_sf2 * _sf2;
    QList<EnsembleGroupes> _listeEnsembles;
    Bloc _currentBloc;

    void changeBloc(QString bloc);
    void addOpcode(QString opcode, QString value);
};

#endif // IMPORT_SFZ_H
