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

#include "sfzparametergroupassembly.h"
#include "soundfontmanager.h"

void SfzParameterGroupAssembly::moveOpcodesInGlobal(SfzParameterGroup &globalZone)
{
    _paramGlobaux.mergeIfNotDefined(globalZone);
}

void SfzParameterGroupAssembly::moveKeynumInSamples(SfzParameter::OpCode opCodeKeynum, SfzParameter::OpCode opCodeBase)
{
    if (_paramGlobaux.isDefined(opCodeKeynum))
    {
        double valueKeynum = _paramGlobaux.getDoubleValue(opCodeKeynum);
        double valueBase = 0.001;
        if (_paramGlobaux.isDefined(opCodeBase))
            valueBase = _paramGlobaux.getDoubleValue(opCodeBase);
        for (int i = 0; i < _listeDivisions.size(); i++)
        {
            if (!_listeDivisions.at(i).isDefined(opCodeKeynum))
                _listeDivisions[i] << SfzParameter(opCodeKeynum, valueKeynum);
            if (!_listeDivisions.at(i).isDefined(opCodeBase))
                _listeDivisions[i] << SfzParameter(opCodeBase, valueBase);
        }

        _paramGlobaux.removeOpCode(opCodeKeynum);
    }
}

void SfzParameterGroupAssembly::moveOpcodeInSamples(SfzParameter::OpCode opcode, QVariant::Type type)
{
    switch (type)
    {
    case QVariant::Double:
        if (_paramGlobaux.isDefined(opcode))
        {
            double value = _paramGlobaux.getDoubleValue(opcode);
            for (int i = 0; i < _listeDivisions.size(); i++)
                if (!_listeDivisions.at(i).isDefined(opcode))
                    _listeDivisions[i] << SfzParameter(opcode, value);
        }
        break;
    case QVariant::Int:
        if (_paramGlobaux.isDefined(opcode))
        {
            int value = _paramGlobaux.getIntValue(opcode);
            for (int i = 0; i < _listeDivisions.size(); i++)
                if (!_listeDivisions.at(i).isDefined(opcode))
                    _listeDivisions[i] << SfzParameter(opcode, value);
        }
        break;
    case QVariant::String:
        if (_paramGlobaux.isDefined(opcode))
        {
            QString value = _paramGlobaux.getStrValue(opcode);
            for (int i = 0; i < _listeDivisions.size(); i++)
                if (!_listeDivisions.at(i).isDefined(opcode))
                    _listeDivisions[i] << SfzParameter(opcode, value);
        }
        break;
    default:
        break;
    }
    _paramGlobaux.removeOpCode(opcode);
}

void SfzParameterGroupAssembly::moveModInSamples()
{
    QList<SfzParameter::OpCode> listTmp;
    listTmp << SfzParameter::op_modEnvToTon
            << SfzParameter::op_pitcheg_attack
            << SfzParameter::op_pitcheg_decay
            << SfzParameter::op_pitcheg_delay
            << SfzParameter::op_pitcheg_hold
            << SfzParameter::op_pitcheg_release
            << SfzParameter::op_pitcheg_sustain;
    moveModInSamples(listTmp);

    listTmp.clear();
    listTmp << SfzParameter::op_modEnvToFilter
            << SfzParameter::op_fileg_attack
            << SfzParameter::op_fileg_decay
            << SfzParameter::op_fileg_delay
            << SfzParameter::op_fileg_hold
            << SfzParameter::op_fileg_release
            << SfzParameter::op_fileg_sustain;
    moveModInSamples(listTmp);

    listTmp.clear();
    listTmp << SfzParameter::op_modLFOtoVolume
            << SfzParameter::op_modLFOdelay
            << SfzParameter::op_modLFOfreq;
    moveModInSamples(listTmp);

    listTmp.clear();
    listTmp << SfzParameter::op_modLFOtoFilter
            << SfzParameter::op_filLFOdelay
            << SfzParameter::op_filLFOfreq;
    moveModInSamples(listTmp);
}

void SfzParameterGroupAssembly::moveModInSamples(QList<SfzParameter::OpCode> opCodeList)
{
    for (int i = 0; i < _listeDivisions.size(); i++)
    {
        for (int j = 0; j < opCodeList.size(); j++)
        {
            SfzParameter::OpCode opCode = opCodeList.at(j);
            if (!_listeDivisions.at(i).isDefined(opCode) && _paramGlobaux.isDefined(opCode))
            {
                // On regarde si d'autres opcodes associés sont présents
                bool isPresent = false;
                for (int k = 0; k < opCodeList.size(); k++)
                    if (k != j)
                        isPresent |= _listeDivisions.at(i).isDefined(opCodeList.at(k));
                if (isPresent)
                {
                    if (opCode == SfzParameter::op_modEnvToTon || opCode == SfzParameter::op_modEnvToFilter ||
                            opCode == SfzParameter::op_modLFOtoVolume || opCode == SfzParameter::op_modLFOtoFilter)
                        _listeDivisions[i] << SfzParameter(opCode, _paramGlobaux.getIntValue(opCode));
                    else
                        _listeDivisions[i] << SfzParameter(opCode, _paramGlobaux.getDoubleValue(opCode));
                }
            }
        }
    }
}

void SfzParameterGroupAssembly::adjustVolume(double offset)
{
    _paramGlobaux.adjustVolume(offset);
    for (int i = 0; i < _listeDivisions.size(); i++)
        _listeDivisions[i].adjustVolume(offset);
}

void SfzParameterGroupAssembly::adjustStereoVolumeAndCorrection(QString path)
{
    for (int i = 0; i < _listeDivisions.size(); i++)
        _listeDivisions[i].adjustStereoVolumeAndCorrection(path, _paramGlobaux.getIntValue(SfzParameter::op_tuningFine));
}

void SfzParameterGroupAssembly::adjustModulationVolume()
{
    double correctionGlobale = 0;
    if (_paramGlobaux.isDefined(SfzParameter::op_modLFOtoVolume))
    {
        correctionGlobale = qAbs(_paramGlobaux.getDoubleValue(SfzParameter::op_modLFOtoVolume));
        _paramGlobaux.adjustVolume(-correctionGlobale);
    }
    for (int i = 0; i < _listeDivisions.size(); i++)
    {
        double correction;
        if (_listeDivisions.at(i).isDefined(SfzParameter::op_modLFOtoVolume))
            correction = qAbs(_listeDivisions.at(i).getDoubleValue(SfzParameter::op_modLFOtoVolume));
        else
            correction = correctionGlobale;
        if (correction != 0)
            _listeDivisions[i].adjustVolume(-correction);
    }
}

void SfzParameterGroupAssembly::checkSampleValid(QString path)
{
    int size = _listeDivisions.size();
    for (int i = size - 1; i >= 0; i--)
        if (!_listeDivisions[i].sampleValid(path))
            _listeDivisions.removeAt(i);
}

void SfzParameterGroupAssembly::checkFilter()
{
    _paramGlobaux.checkFilter();
    for (int i = 0; i < _listeDivisions.size(); i++)
        _listeDivisions[i].checkFilter();
}

void SfzParameterGroupAssembly::simplifyAttenuation()
{
    bool attenuationUnique = true;
    double attenuation = -1;
    double defaultAttenuation = _paramGlobaux.getDoubleValue(SfzParameter::op_volume);
    for (int i = 0; i < _listeDivisions.size(); i++)
    {
        double dTmp = defaultAttenuation;
        if (_listeDivisions[i].isDefined(SfzParameter::op_volume))
            dTmp = _listeDivisions[i].getDoubleValue(SfzParameter::op_volume);
        if (attenuation == -1)
            attenuation = dTmp;
        else
        {
            if (attenuation != dTmp)
                attenuationUnique = false;
        }
    }
    if (attenuationUnique)
    {
        for (int i = 0; i < _listeDivisions.size(); i++)
            _listeDivisions[i].removeOpCode(SfzParameter::op_volume);
        if (attenuation == 0)
            _paramGlobaux.removeOpCode(SfzParameter::op_volume);
        else
        {
            if (_paramGlobaux.isDefined(SfzParameter::op_volume))
                _paramGlobaux.adjustVolume(_paramGlobaux.getDoubleValue(SfzParameter::op_volume) - attenuation);
            else
                _paramGlobaux << SfzParameter(SfzParameter::op_volume, attenuation);
        }
    }
}

bool SfzParameterGroupAssembly::isChannel10()
{
    return (_paramGlobaux.getIntValue(SfzParameter::op_chanMin) == 10 &&
            _paramGlobaux.getIntValue(SfzParameter::op_chanMax) == 10);
}

double SfzParameterGroupAssembly::getAmpliMax()
{
    double ampliMax = _paramGlobaux.getDoubleValue(SfzParameter::op_volume);
    for (int i = 0; i < _listeDivisions.size(); i++)
        ampliMax = qMax(ampliMax, _listeDivisions.at(i).getDoubleValue(SfzParameter::op_volume));
    return ampliMax;
}

void SfzParameterGroupAssembly::decode(SoundfontManager * sf2, EltID idInst, QString pathSfz)
{
    // Remplissage des paramètres globaux
    _paramGlobaux.decode(sf2, idInst);

    // Lien avec samples
    EltID idInstSmpl = idInst;
    idInstSmpl.typeElement = elementInstSmpl;
    EltID idSmpl = idInst;
    idSmpl.typeElement = elementSmpl;
    AttributeValue val;
    for (int i = 0; i < _listeDivisions.size(); i++)
    {
        // Création des samples si besoin et récupération de leur index
        QList<int> listeIndexSmpl = _listeDivisions.at(i).getSampleIndex(sf2, idInst, pathSfz);

        // Tranformation des offsets si présents
        if (!listeIndexSmpl.isEmpty())
        {
            idSmpl.indexElt = listeIndexSmpl.first();
            _listeDivisions[i].adaptOffsets(sf2->get(idSmpl, champ_dwStartLoop).dwValue,
                                            sf2->get(idSmpl, champ_dwEndLoop).dwValue,
                                            sf2->get(idSmpl, champ_dwLength).dwValue);
        }

        if (_listeDivisions.at(i).isDefined(SfzParameter::op_pan) && listeIndexSmpl.size() == 2)
        {
            double pan = _listeDivisions.at(i).getDoubleValue(SfzParameter::op_pan);
            if (qAbs(pan) >= 99.9)
            {
                // Passage en mono
                if (pan < 0)
                    listeIndexSmpl.removeAt(1);
                else
                    listeIndexSmpl.removeAt(0);
            }
        }

        if (listeIndexSmpl.size() == 1) // Mono sample
        {
            // Création InstSmpl
            idInstSmpl.indexElt2 = sf2->add(idInstSmpl);

            // Lien avec le sample
            val.wValue = listeIndexSmpl.first();
            sf2->set(idInstSmpl, champ_sampleID, val);

            // Remplissage paramètres de la division
            _listeDivisions.at(i).decode(sf2, idInstSmpl);

            if (_listeDivisions.at(i).isDefined(SfzParameter::op_pan))
            {
                val.shValue = 5 * _listeDivisions.at(i).getDoubleValue(SfzParameter::op_pan);
                sf2->set(idInstSmpl, champ_pan, val);
            }
        }
        else if (listeIndexSmpl.size() == 2) // Sample stereo
        {
            // Gestion width / position
            double width = 500;
            if (_listeDivisions.at(i).isDefined(SfzParameter::op_width))
                width = 5. * _listeDivisions.at(i).getDoubleValue(SfzParameter::op_width);
            double position = 0;
            if (_listeDivisions.at(i).isDefined(SfzParameter::op_position))
                position = _listeDivisions.at(i).getDoubleValue(SfzParameter::op_position) / 100.;
            if (position < 0)
                position = -qAbs(position * (500 - qAbs(width)));
            else
                position = qAbs(position * (500 - qAbs(width)));

            // Gestion pan
            double attenuation = 0;
            int panDefined = -1;
            if (_listeDivisions.at(i).isDefined(SfzParameter::op_pan))
            {
                double pan = _listeDivisions.at(i).getDoubleValue(SfzParameter::op_pan);
                attenuation = -SfzParameterGroup::percentToDB(100 - qAbs(pan)) * 1.7;
                if (pan < 0)
                    panDefined = 1;
                else if (pan > 0)
                    panDefined = 0;
            }

            for (int j = 0; j < listeIndexSmpl.size(); j++)
            {
                // Création InstSmpl
                idInstSmpl.indexElt2 = sf2->add(idInstSmpl);

                // Lien avec le sample
                val.wValue = listeIndexSmpl.at(j);
                sf2->set(idInstSmpl, champ_sampleID, val);

                // Pan
                if (panDefined == j)
                {
                    val.shValue = qRound(10. * attenuation);
                    sf2->set(idInstSmpl, champ_initialAttenuation, val);
                }

                // Remplissage paramètres de la division
                _listeDivisions.at(i).decode(sf2, idInstSmpl);

                // Width, position
                if (j == 0)
                    val.shValue = -width + position;
                else
                    val.shValue = width + position;
                sf2->set(idInstSmpl, champ_pan, val);
            }
        }
    }
}
