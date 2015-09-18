/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2015 Davy Triponney                                **
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
#include <QVariant>
#include "sf2_types.h"
#include "qmath.h"

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
        op_chanMin,
        op_chanMax,
        op_rootKey,
        op_exclusiveClass,
        op_off_by,
        op_tuningFine,
        op_tuningCoarse,
        op_delay,
        op_offset,
        op_end,
        op_loop_start,
        op_loop_end,
        op_loop_mode,
        op_volume,
        op_pan,
        op_width,
        op_position,
        op_tuningScale,
        op_chorus,
        op_reverb,
        op_filterType,
        op_filterFreq,
        op_filterQ,
        op_amp_velcurve_1,
        op_amp_velcurve_127,

        // Enveloppe volume
        op_ampeg_delay,
        op_ampeg_attack,
        op_ampeg_hold,
        op_ampeg_decay,
        op_ampeg_sustain,
        op_ampeg_release,
        op_noteToVolEnvHold,
        op_noteToVolEnvDecay,

        // LFO volume
        op_modLFOdelay,
        op_modLFOfreq,
        op_modLFOtoVolume,

        // Enveloppe pitch
        op_pitcheg_delay,
        op_pitcheg_attack,
        op_pitcheg_hold,
        op_pitcheg_decay,
        op_pitcheg_sustain,
        op_pitcheg_release,
        op_noteToModEnvHold,
        op_noteToModEnvDecay,
        op_modEnvToTon,

        // LFO pitch
        op_vibLFOdelay,
        op_vibLFOfreq,
        op_vibLFOtoTon,

        // Enveloppe filtre (si équivalent à enveloppe pitch, ou si enveloppe pitch non utilisé)
        op_fileg_delay,
        op_fileg_attack,
        op_fileg_hold,
        op_fileg_decay,
        op_fileg_sustain,
        op_fileg_release,
        op_modEnvToFilter,
        op_fileg_holdcc133,
        op_fileg_decaycc133,

        // LFO filtre (si équivalent à LFO volume ou si LFO volume non utilisé)
        op_filLFOdelay,
        op_filLFOfreq,
        op_modLFOtoFilter
    };

    Parametre(QString opcode, QString valeur);
    Parametre(OpCode opcode, int valeur) :
        _opcode(opcode),
        _intValue(valeur),
        _dblValue(0.)
    {}
    Parametre(OpCode opcode, double valeur) :
        _opcode(opcode),
        _intValue(0),
        _dblValue(valeur)
    {}
    Parametre(OpCode opcode, QString valeur) :
        _opcode(opcode),
        _intValue(0),
        _dblValue(0),
        _strValue(valeur)
    {}
    OpCode  getOpCode()      const       { return _opcode;   }
    int     getIntValue()    const       { return _intValue; }
    double  getDoubleValue() const       { return _dblValue; }
    QString getStringValue() const       { return _strValue; }
    void    setIntValue(int value)       { _intValue = value; }
    void    setDoubleValue(double value) { _dblValue = value; }

private:
    OpCode  _opcode;
    int     _intValue;
    double  _dblValue;
    QString _strValue;
};

class GroupeParametres
{
public:
    // Chargement
    GroupeParametres() {}
    void operator << (Parametre param) { _listeParam << param; }

    // Décodage
    void decode(Pile_sf2 * sf2, EltID idElt) const;
    QList<int> getSampleIndex(Pile_sf2 * sf2, EltID idElt, QString pathSfz) const;
    void adaptOffsets(int startLoop, int endLoop, int length);
    void adjustStereoVolumeAndCorrection(QString path, int defaultCorrection);
    bool sampleValid(QString path) const;
    void checkFilter();
    void adjustVolume(double offset);
    void adjustCorrection(int offset, int defaultCorrection);
    void removeOpCode(Parametre::OpCode opcode)
    {
        for (int i = _listeParam.size() - 1; i >= 0; i--)
            if (_listeParam.at(i).getOpCode() == opcode)
                _listeParam.removeAt(i);
    }

    // Lecture
    bool isDefined(Parametre::OpCode opcode) const
    {
        bool bRet = false;
        for (int i = 0; i < _listeParam.size(); i++)
            if (_listeParam.at(i).getOpCode() == opcode)
                bRet = true;
        return bRet;
    }
    int getIntValue(Parametre::OpCode opcode) const
    {
        int iRet = 0;
        for (int i = 0; i < _listeParam.size(); i++)
            if (_listeParam.at(i).getOpCode() == opcode)
                iRet = _listeParam.at(i).getIntValue();
        return iRet;
    }
    double getDoubleValue(Parametre::OpCode opcode) const
    {
        double dRet = 0;
        for (int i = 0; i < _listeParam.size(); i++)
            if (_listeParam.at(i).getOpCode() == opcode)
                dRet = _listeParam.at(i).getDoubleValue();
        return dRet;
    }
    QString getStrValue(Parametre::OpCode opcode) const
    {
        QString strRet = "";
        for (int i = 0; i < _listeParam.size(); i++)
            if (_listeParam.at(i).getOpCode() == opcode)
                strRet = _listeParam.at(i).getStringValue();
        return strRet;
    }
    static double percentToDB(double percent) { return log10(percent / 100.) * 20.; }

private:
    QList<Parametre> _listeParam;
    static QStringList getFullPath(QString base, QStringList directories);
    static double log2m1200(double value) { return 1200. * qLn(value) / 0.69314718056 /* ln(2) */; }
    static double d1200e2(int value) { return qPow(2., value / 1200.); }
    static void addSeconds(double value, Champ champ, Pile_sf2 * sf2, EltID id);
    void getKeynumValues(double &baseValue, int &keynum, Parametre::OpCode opCodeKeynum, Parametre::OpCode opCodeBase) const;
    static QString getName(QString name, int maxCharacters, int suffixNumber, QString suffix = "");
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
    void moveKeynumInSamples(Parametre::OpCode opCodeKeynum, Parametre::OpCode opCodeBase);
    void moveOpcodeInSamples(Parametre::OpCode opcode, QVariant::Type type);
    void moveModInSamples();
    void moveModInSamples(QList<Parametre::OpCode> opCodeList);
    void checkSampleValid(QString path);
    void checkFilter();
    void simplifyAttenuation();
    void adjustStereoVolumeAndCorrection(QString path);
    void adjustModulationVolume();
    bool isChannel10();
    double getAmpliMax();
    void adjustVolume(double offset);

    // Décodage
    void decode(Pile_sf2 * sf2, EltID idInst, QString pathSfz);

private:
    GroupeParametres _paramGlobaux;
    QList<GroupeParametres> _listeDivisions;
};


class ImportSfz
{
public:
    ImportSfz(Pile_sf2 *sf2);
    void import(QString fileName, int *numSf2);

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
    QString getNomInstrument(QString filePath, int &numBank, int &numPreset);
};

#endif // IMPORT_SFZ_H
