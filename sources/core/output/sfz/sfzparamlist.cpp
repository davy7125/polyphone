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

#include "sfzparamlist.h"
#include "soundfontmanager.h"

int SfzParamList::_boucleGlobale = -2;

SfzParamList::SfzParamList(SoundfontManager * sf2, EltID id)
{
    if (id.typeElement == elementPrstInst)
    {
        // Chargement d'un preset
        id.typeElement = elementPrstInstGen;
        load(sf2, id);

        // Prise en compte des paramètres globaux
        id.typeElement = elementPrstGen;
        load(sf2, id);
    }
}

SfzParamList::SfzParamList(SoundfontManager * sf2, SfzParamList * paramPrst, EltID idInst)
{
    if (idInst.typeElement == elementInst)
    {
        // Chargement des paramètres globaux d'un instrument
        idInst.typeElement = elementInstGen;
        load(sf2, idInst);

        idInst.typeElement = elementInst;
        if (sf2->isSet(idInst, champ_sampleModes))
            _boucleGlobale = sf2->get(idInst, champ_sampleModes).wValue;
        else
        {
            // Toutes les divisions ont-elles le même paramètre de bouclage ?
            idInst.typeElement = elementInstSmpl;
            _boucleGlobale = -2;
            foreach (int i, sf2->getSiblings(idInst))
            {
                idInst.indexElt2 = i;
                int valTmp = 0;
                if (sf2->isSet(idInst, champ_sampleModes))
                    valTmp = sf2->get(idInst, champ_sampleModes).wValue;
                if (_boucleGlobale == -2)
                    _boucleGlobale = valTmp;
                else if (_boucleGlobale != valTmp)
                    _boucleGlobale = -1;
            }
            idInst.typeElement = elementInst;
        }

        if (_boucleGlobale >= 0)
        {
            // Ajustement de la boucle globale
            if (!_listeChamps.contains(champ_sampleModes))
            {
                _listeChamps << champ_sampleModes;
                _listeValeurs << _boucleGlobale;
            }
            else
                _listeValeurs[_listeChamps.indexOf(champ_sampleModes)] = _boucleGlobale;
        }
        else
        {
            // Pas de boucle si non défini
            if (!_listeChamps.contains(champ_sampleModes))
            {
                _listeChamps << champ_sampleModes;
                _listeValeurs << 0;
                _boucleGlobale = 0;
            }
            else
                _boucleGlobale = _listeValeurs.at(_listeChamps.indexOf(champ_sampleModes));
        }
    }
    else if (idInst.typeElement == elementInstSmpl)
    {
        // Chargement des paramètres d'une zone dans un instrument
        idInst.typeElement = elementInstSmplGen;
        load(sf2, idInst);

        // Lecture du sample associé
        EltID idSmpl = idInst;
        idSmpl.typeElement = elementSmpl;
        idInst.typeElement = elementInstSmpl;
        idSmpl.indexElt = sf2->get(idInst, champ_sampleID).wValue;

        if (!_listeChamps.contains(champ_overridingRootKey))
        {
            EltID instGlob = idInst;
            instGlob.typeElement = elementInst;
            if (!sf2->isSet(instGlob, champ_overridingRootKey))
            {
                _listeChamps << champ_overridingRootKey;
                _listeValeurs << sf2->get(idSmpl, champ_byOriginalPitch).bValue;
            }
        }

        if (!_listeChamps.contains(champ_fineTune))
        {
            int fineTuneSample = sf2->get(idSmpl, champ_chPitchCorrection).cValue;
            if (fineTuneSample != 0)
            {
                _listeChamps << champ_fineTune;
                EltID idInstGlobal = idInst;
                idInstGlobal.typeElement = elementInst;
                _listeValeurs << sf2->get(idInstGlobal, champ_fineTune).shValue + fineTuneSample;
            }
        }
        else
        {
            _listeValeurs[_listeChamps.indexOf(champ_fineTune)] +=
                    sf2->get(idSmpl, champ_chPitchCorrection).cValue;
        }

        // Suppression sample mode si égal à la division globale
        if (_listeChamps.contains(champ_sampleModes))
        {
            int index = _listeChamps.indexOf(champ_sampleModes);
            if (_listeValeurs.at(index) == _boucleGlobale)
            {
                _listeChamps.removeAt(index);
                _listeValeurs.removeAt(index);
            }
        }

        // Adaptation des offsets
        mix(champ_startAddrsCoarseOffset, champ_startAddrsOffset);
        mix(champ_endAddrsCoarseOffset, champ_endAddrsOffset, sf2->get(idSmpl, champ_dwLength).dwValue);
        mix(champ_startloopAddrsCoarseOffset, champ_startloopAddrsOffset, sf2->get(idSmpl, champ_dwStartLoop).dwValue);
        mix(champ_endloopAddrsCoarseOffset, champ_endloopAddrsOffset, sf2->get(idSmpl, champ_dwEndLoop).dwValue);

        // Suppression début / fin de loop si 0 - 0
        if (_listeChamps.contains(champ_startAddrsOffset) && _listeChamps.contains(champ_endloopAddrsOffset))
        {
            int startLoop = _listeValeurs.at(_listeChamps.indexOf(champ_startloopAddrsOffset));
            int endLoop = _listeValeurs.at(_listeChamps.indexOf(champ_endloopAddrsOffset));
            if (startLoop == endLoop)
            {
                int index = _listeChamps.indexOf(champ_startloopAddrsOffset);
                _listeChamps.removeAt(index);
                _listeValeurs.removeAt(index);
                index = _listeChamps.indexOf(champ_endloopAddrsOffset);
                _listeChamps.removeAt(index);
                _listeValeurs.removeAt(index);
            }
        }

        // Gestion de la note fixe
        if (_listeChamps.contains(champ_keynum))
        {
            int indexKeynum = _listeChamps.indexOf(champ_keynum);
            int delta = _listeValeurs.at(indexKeynum) -
                    _listeValeurs.at(_listeChamps.indexOf(champ_overridingRootKey));

            // Equivalence en utilisant scaleTuning / rootkey / coarse tune
            if (_listeChamps.contains(champ_scaleTuning))
                _listeValeurs[_listeChamps.indexOf(champ_scaleTuning)] = 0;
            else
            {
                _listeChamps << champ_scaleTuning;
                _listeValeurs << 0;
            }
            _listeValeurs[_listeChamps.indexOf(champ_overridingRootKey)] =
                    _listeValeurs.at(indexKeynum);
            if (_listeChamps.contains(champ_coarseTune))
                _listeValeurs[_listeChamps.indexOf(champ_coarseTune)] += delta;
            else
            {
                _listeChamps << champ_coarseTune;
                _listeValeurs << delta;
            }

            _listeChamps.removeAt(indexKeynum);
            _listeValeurs.removeAt(indexKeynum);
        }

        // Adaptation des keynum2...
        adaptKeynum2();

        // Adaptation LFO
        adaptLfo(sf2, idInst);

        // Atténuation du volume si défini uniquement dans la division globale
        if (!_listeChamps.contains(champ_initialAttenuation))
        {
            EltID instGlob = idInst;
            instGlob.typeElement = elementInst;
            if (sf2->isSet(instGlob, champ_initialAttenuation))
            {
                _listeChamps << champ_initialAttenuation;
                _listeValeurs << Attribute::toRealValue(champ_initialAttenuation, false, sf2->get(instGlob, champ_initialAttenuation));
            }
        }
    }

    // Fusion des 2 listes de paramètres
    if (idInst.typeElement == elementInst)
    {
        for (int i = 0; i < paramPrst->_listeChamps.size(); i++)
            fusion(paramPrst->_listeChamps.at(i), paramPrst->_listeValeurs.at(i));
    }
    else if (idInst.typeElement == elementInstSmpl)
    {
        // On fusionne uniquement avec les éléments présents
        for (int i = 0; i < paramPrst->_listeChamps.size(); i++)
            if (_listeChamps.contains(paramPrst->_listeChamps.at(i)))
                fusion(paramPrst->_listeChamps.at(i), paramPrst->_listeValeurs.at(i));
    }

    // On tente d'éliminer le champ modLfoToPitch si vib LFO est disponible
    if (_listeChamps.contains(champ_modLfoToPitch) && !_listeChamps.contains(champ_vibLfoToPitch))
    {
        int index = _listeChamps.indexOf(champ_modLfoToPitch);
        _listeChamps << champ_vibLfoToPitch;
        _listeValeurs << _listeValeurs.at(index);

        // Recopie des paramètres
        if (_listeChamps.contains(champ_delayModLFO))
        {
            int index2 = _listeChamps.indexOf(champ_delayModLFO);
            if (_listeChamps.contains(champ_delayVibLFO))
                _listeValeurs[_listeChamps.indexOf(champ_delayVibLFO)] = _listeValeurs[index2];
            else
            {
                _listeChamps << champ_delayVibLFO;
                _listeValeurs << _listeValeurs[index2];
            }
        }
        if (_listeChamps.contains(champ_freqModLFO))
        {
            int index2 = _listeChamps.indexOf(champ_freqModLFO);
            if (_listeChamps.contains(champ_freqVibLFO))
                _listeValeurs[_listeChamps.indexOf(champ_freqVibLFO)] = _listeValeurs[index2];
            else
            {
                _listeChamps << champ_freqVibLFO;
                _listeValeurs << _listeValeurs[index2];
            }
        }

        _listeChamps.removeAt(index);
        _listeValeurs.removeAt(index);

        // Suppression complète du mod lfo ?
        if (!_listeChamps.contains(champ_modLfoToVolume) && !_listeChamps.contains(champ_modLfoToFilterFc))
        {
            if (_listeChamps.contains(champ_delayModLFO))
            {
                int index2 = _listeChamps.indexOf(champ_delayModLFO);
                _listeChamps.removeAt(index2);
                _listeValeurs.removeAt(index2);
            }
            if (_listeChamps.contains(champ_freqModLFO))
            {
                int index2 = _listeChamps.indexOf(champ_freqModLFO);
                _listeChamps.removeAt(index2);
                _listeValeurs.removeAt(index2);
            }
        }
    }

    // Valeurs par défaut
    if (idInst.typeElement == elementInst)
    {
        // Attaque par défaut si non définie
        if (!_listeChamps.contains(champ_attackVolEnv))
        {
            _listeChamps << champ_attackVolEnv;
            _listeValeurs << Attribute::getDefaultRealValue(champ_attackVolEnv, false);
        }

        // Fréquences par défaut si non définies
        if (!_listeChamps.contains(champ_freqModLFO))
        {
            _listeChamps << champ_freqModLFO;
            _listeValeurs << Attribute::getDefaultRealValue(champ_freqModLFO, false);
        }
        if (!_listeChamps.contains(champ_freqVibLFO))
        {
            _listeChamps << champ_freqVibLFO;
            _listeValeurs << Attribute::getDefaultRealValue(champ_freqVibLFO, false);
        }

        // Fréquence de coupure par défaut si non définie
        if (!_listeChamps.contains(champ_initialFilterFc))
        {
            _listeChamps << champ_initialFilterFc;
            _listeValeurs << Attribute::getDefaultRealValue(champ_initialFilterFc, false);
        }
    }

    // Limites
    for (int i = 0; i < _listeChamps.size(); i++)
        _listeValeurs[i] = limit(_listeValeurs.at(i), _listeChamps.at(i));

    // Correction volume si modLfoToVolume est actif
    double correction = 0;
    idInst.typeElement = elementInst;
    if (_listeChamps.contains(champ_modLfoToVolume))
        correction = qAbs(_listeValeurs.at(_listeChamps.indexOf(champ_modLfoToVolume)));
    else if (sf2->isSet(idInst, champ_modLfoToVolume))
        correction = qAbs((double)sf2->get(idInst, champ_modLfoToVolume).shValue / 10.);
    if (correction != 0)
    {
        if (_listeChamps.contains(champ_initialAttenuation))
            _listeValeurs[_listeChamps.indexOf(champ_initialAttenuation)] += correction;
        else
        {
            _listeChamps << champ_initialAttenuation;
            _listeValeurs << correction / DB_SF2_TO_SFZ;
        }
    }

    // Ordre
    prepend(champ_velRange);
    prepend(champ_keynum);
    prepend(champ_overridingRootKey);
    prepend(champ_keyRange);
}

void SfzParamList::adaptKeynum2()
{
    int minKey = 0, maxKey = 127;
    if (_listeChamps.contains(champ_keyRange))
    {
        double keyRange = _listeValeurs.at(_listeChamps.indexOf(champ_keyRange));
        minKey = qRound(keyRange / 1000);
        maxKey = qRound(keyRange - 1000 * minKey);
    }

    adaptKeynum2(minKey, maxKey, champ_decayModEnv, champ_keynumToModEnvDecay);
    adaptKeynum2(minKey, maxKey, champ_holdModEnv, champ_keynumToModEnvHold);
    adaptKeynum2(minKey, maxKey, champ_decayVolEnv, champ_keynumToVolEnvDecay);
    adaptKeynum2(minKey, maxKey, champ_holdVolEnv, champ_keynumToVolEnvHold);
}
void SfzParamList::adaptKeynum2(int minKey, int maxKey, AttributeType champBase, AttributeType champKeynum)
{
    double valBase = 0.001;
    double valMin, valMax;
    double keyNum;

    int indexKeynum = _listeChamps.indexOf(champKeynum);
    if (indexKeynum != -1)
    {
        keyNum = _listeValeurs.at(indexKeynum);

        int indexValBase = _listeChamps.indexOf(champBase);
        if (indexValBase != -1)
            valBase = _listeValeurs.at(indexValBase);

        valMin = getValKeynum(valBase, minKey, keyNum);
        valMax = getValKeynum(valBase, maxKey, keyNum);
        if (minKey == maxKey)
        {
            if (indexValBase == -1)
            {
                _listeChamps << champBase;
                _listeValeurs << valMin;
            }
            else
                _listeValeurs[indexValBase] = valMin;
            _listeValeurs.removeAt(indexKeynum);
            _listeChamps.removeAt(indexKeynum);
        }
        else
        {
            keyNum = 127. * (valMin - valMax) / (double)(minKey - maxKey);
            valBase = (valMax * minKey - valMin * maxKey) / (double)(minKey - maxKey);
            _listeValeurs[indexKeynum] = keyNum;
            if (indexValBase == -1)
            {
                _listeChamps << champBase;
                _listeValeurs << valBase;
            }
            else
                _listeValeurs[indexValBase] = valBase;
        }
    }
}

void SfzParamList::adaptLfo(SoundfontManager * sf2, EltID idInstSmpl)
{
    // On se trouve dans une division d'un instrument
    EltID idInst = idInstSmpl;
    idInst.typeElement = elementInst;

    // Reprise des valeurs de la division globale
    if (_listeChamps.contains(champ_modLfoToPitch) || _listeChamps.contains(champ_modLfoToFilterFc) ||
            _listeChamps.contains(champ_modLfoToVolume) || _listeChamps.contains(champ_delayModLFO) ||
            _listeChamps.contains(champ_freqModLFO))
    {
        if (!_listeChamps.contains(champ_delayModLFO) && sf2->isSet(idInst, champ_delayModLFO))
        {
            _listeChamps << champ_delayModLFO;
            _listeValeurs << Attribute::toRealValue(champ_delayModLFO, false, sf2->get(idInst, champ_delayModLFO));
        }
        if (!_listeChamps.contains(champ_freqModLFO) && sf2->isSet(idInst, champ_freqModLFO))
        {
            _listeChamps << champ_freqModLFO;
            _listeValeurs << Attribute::toRealValue(champ_freqModLFO, false, sf2->get(idInst, champ_freqModLFO));
        }
        if (!_listeChamps.contains(champ_modLfoToPitch) && sf2->isSet(idInst, champ_modLfoToPitch))
        {
            _listeChamps << champ_modLfoToPitch;
            _listeValeurs << Attribute::toRealValue(champ_modLfoToPitch, false, sf2->get(idInst, champ_modLfoToPitch));
        }
        if (!_listeChamps.contains(champ_modLfoToFilterFc) && sf2->isSet(idInst, champ_modLfoToFilterFc))
        {
            _listeChamps << champ_modLfoToFilterFc;
            _listeValeurs << Attribute::toRealValue(champ_modLfoToFilterFc, false, sf2->get(idInst, champ_modLfoToFilterFc));
        }
        if (!_listeChamps.contains(champ_modLfoToVolume) && sf2->isSet(idInst, champ_modLfoToVolume))
        {
            _listeChamps << champ_modLfoToVolume;
            _listeValeurs << Attribute::toRealValue(champ_modLfoToVolume, false, sf2->get(idInst, champ_modLfoToVolume));
        }
    }
    if (_listeChamps.contains(champ_vibLfoToPitch) || _listeChamps.contains(champ_delayVibLFO) ||
            _listeChamps.contains(champ_freqVibLFO))
    {
        if (!_listeChamps.contains(champ_delayVibLFO) && sf2->isSet(idInst, champ_delayVibLFO))
        {
            _listeChamps << champ_delayVibLFO;
            _listeValeurs << Attribute::toRealValue(champ_delayVibLFO, false, sf2->get(idInst, champ_delayVibLFO));
        }
        if (!_listeChamps.contains(champ_freqVibLFO) && sf2->isSet(idInst, champ_freqVibLFO))
        {
            _listeChamps << champ_freqVibLFO;
            _listeValeurs << Attribute::toRealValue(champ_freqVibLFO, false, sf2->get(idInst, champ_freqVibLFO));
        }
        if (!_listeChamps.contains(champ_vibLfoToPitch) && sf2->isSet(idInst, champ_vibLfoToPitch))
        {
            _listeChamps << champ_vibLfoToPitch;
            _listeValeurs << Attribute::toRealValue(champ_vibLfoToPitch, false, sf2->get(idInst, champ_vibLfoToPitch));
        }
    }
}

double SfzParamList::getValKeynum(double valBase, int key, double keynum)
{
    return valBase * qPow(2., (60. - (double)key) * keynum / 1200.);
}

void SfzParamList::prepend(AttributeType champ)
{
    if (_listeChamps.indexOf(champ) != -1)
    {
        int index = _listeChamps.indexOf(champ);
        AttributeType chTmp = _listeChamps.takeAt(index);
        double value = _listeValeurs.takeAt(index);
        _listeChamps.prepend(chTmp);
        _listeValeurs.prepend(value);
    }
}

void SfzParamList::load(SoundfontManager * sf2, EltID id)
{
    // Charge les paramètres, n'écrase pas les valeurs existantes
    bool isPrst = (id.typeElement == elementPrstInstGen || id.typeElement == elementPrstGen);
    EltID parent = id.parent();

    foreach (int champ, sf2->getSiblings(id))
    {
        if (!_listeChamps.contains((AttributeType)champ))
        {
            if (id.typeElement != elementInstGen || (
                        champ != champ_startAddrsCoarseOffset &&
                        champ != champ_startAddrsOffset &&
                        champ != champ_startloopAddrsCoarseOffset &&
                        champ != champ_startloopAddrsOffset &&
                        champ != champ_endAddrsCoarseOffset &&
                        champ != champ_endAddrsOffset &&
                        champ != champ_endloopAddrsCoarseOffset &&
                        champ != champ_endloopAddrsOffset &&
                        champ != champ_keynum &&
                        champ != champ_keynumToModEnvDecay &&
                        champ != champ_keynumToModEnvHold &&
                        champ != champ_keynumToVolEnvDecay &&
                        champ != champ_keynumToVolEnvHold))
            {
                _listeChamps << (AttributeType)champ;
                _listeValeurs << Attribute::toRealValue((AttributeType)champ, isPrst, sf2->get(parent, (AttributeType)champ));
            }
        }
    }

    if (id.typeElement == elementInstSmplGen)
    {
        id.typeElement = elementInst;

        // Chargement des offsets de la division globale
        getGlobalValue(sf2, id, champ_startAddrsCoarseOffset);
        getGlobalValue(sf2, id, champ_startAddrsOffset);
        getGlobalValue(sf2, id, champ_startloopAddrsCoarseOffset);
        getGlobalValue(sf2, id, champ_startloopAddrsOffset);
        getGlobalValue(sf2, id, champ_endAddrsCoarseOffset);
        getGlobalValue(sf2, id, champ_endAddrsOffset);
        getGlobalValue(sf2, id, champ_endloopAddrsCoarseOffset);
        getGlobalValue(sf2, id, champ_endloopAddrsOffset);

        // Chargement de la note fixe de la division globale
        getGlobalValue(sf2, id, champ_keynum);

        // Chargement des keynum2... de la division globale, avec les valeurs modulées
        getGlobalValue(sf2, id, champ_keynumToModEnvDecay);
        if (_listeChamps.contains(champ_keynumToModEnvDecay))
            if (_listeValeurs.at(_listeChamps.indexOf(champ_keynumToModEnvDecay)) != 0)
                getGlobalValue(sf2, id, champ_decayModEnv);
        getGlobalValue(sf2, id, champ_keynumToModEnvHold);
        if (_listeChamps.contains(champ_keynumToModEnvHold))
            if (_listeValeurs.at(_listeChamps.indexOf(champ_keynumToModEnvHold)) != 0)
                getGlobalValue(sf2, id, champ_holdModEnv);
        getGlobalValue(sf2, id, champ_keynumToVolEnvDecay);
        if (_listeChamps.contains(champ_keynumToVolEnvDecay))
            if (_listeValeurs.at(_listeChamps.indexOf(champ_keynumToVolEnvDecay)) != 0)
                getGlobalValue(sf2, id, champ_decayVolEnv);
        getGlobalValue(sf2, id, champ_keynumToVolEnvHold);
        if (_listeChamps.contains(champ_keynumToVolEnvHold))
            if (_listeValeurs.at(_listeChamps.indexOf(champ_keynumToVolEnvHold)) != 0)
                getGlobalValue(sf2, id, champ_holdVolEnv);
    }
}

void SfzParamList::getGlobalValue(SoundfontManager * sf2, EltID id, AttributeType champ)
{
    // Chargement d'une valeur de la division globale
    if (!_listeChamps.contains(champ) && sf2->isSet(id, champ))
    {
        _listeChamps << champ;
        _listeValeurs << Attribute::toRealValue(champ, false, sf2->get(id, champ));
    }
}

void SfzParamList::mix(AttributeType champCoarse, AttributeType champFine, int addValue)
{
    if (_listeChamps.contains(champCoarse))
    {
        int indexCoarse = _listeChamps.indexOf(champCoarse);
        if (_listeChamps.contains(champFine))
        {
            _listeValeurs[_listeChamps.indexOf(champFine)] += _listeValeurs[indexCoarse];
            _listeChamps.removeAt(indexCoarse);
            _listeValeurs.removeAt(indexCoarse);
        }
        else
            _listeChamps[indexCoarse] = champFine;
    }

    if (_listeChamps.contains(champFine))
        _listeValeurs[_listeChamps.indexOf(champFine)] += addValue;
    else
    {
        _listeChamps << champFine;
        _listeValeurs << addValue;
    }
}

void SfzParamList::fusion(AttributeType champ, double value)
{
    int index = _listeChamps.indexOf(champ);
    if (index == -1)
    {
        index = _listeChamps.size();
        _listeChamps.append(champ);
        _listeValeurs.append(Attribute::getDefaultRealValue(champ, false));
    }

    switch (champ)
    {
    case champ_fineTune: case champ_coarseTune: case champ_initialAttenuation: case champ_startAddrsOffset:
    case champ_endAddrsOffset: case champ_startloopAddrsOffset: case champ_endloopAddrsOffset:
    case champ_pan: case champ_scaleTuning: case champ_initialFilterQ: case champ_reverbEffectsSend:
    case champ_chorusEffectsSend: case champ_keynumToVolEnvHold: case champ_keynumToVolEnvDecay:
    case champ_sustainVolEnv: case champ_keynumToModEnvHold: case champ_keynumToModEnvDecay:
    case champ_sustainModEnv: case champ_modEnvToPitch: case champ_modEnvToFilterFc: case champ_modLfoToPitch:
    case champ_modLfoToFilterFc: case champ_vibLfoToPitch: case champ_modLfoToVolume:
        _listeValeurs[index] += value;
        break;
    case champ_keynum: case champ_overridingRootKey: case champ_velocity: case champ_exclusiveClass:
    case champ_sampleModes:
        _listeValeurs[index] = value;
        break;
    case champ_initialFilterFc: case champ_delayVolEnv: case champ_attackVolEnv: case champ_holdVolEnv:
    case champ_decayVolEnv: case champ_releaseVolEnv: case champ_delayModEnv: case champ_attackModEnv:
    case champ_holdModEnv: case champ_decayModEnv: case champ_releaseModEnv: case champ_delayModLFO:
    case champ_freqModLFO: case champ_delayVibLFO: case champ_freqVibLFO:
        _listeValeurs[index] *= value;
        break;
    case champ_keyRange: case champ_velRange:{
        int minNew = qRound(value / 1000.);
        int maxNew = qRound(value - 1000. * minNew);
        int minOld = qRound(_listeValeurs.at(index) / 1000.);
        int maxOld = qRound(_listeValeurs.at(index) - 1000. * minOld);
        if (minNew > maxNew)
        {
            int tmp = maxNew;
            maxNew = minNew;
            minNew = tmp;
        }
        if (minOld > maxOld)
        {
            int tmp = maxOld;
            maxOld = minOld;
            minOld = tmp;
        }
        if (maxNew < minOld || maxOld < minNew)
        {
            minNew = 0;
            maxNew = 0;
        }
        else
        {
            minNew = qMax(minOld, minNew);
            maxNew = qMin(maxOld, maxNew);
        }
        _listeValeurs[index] = 1000. * minNew + maxNew;
    }break;
    default:
        break;
    }
}

double SfzParamList::limit(double val, AttributeType champ)
{
    double min = 0;
    double max = 0;

    switch (champ)
    {
    case champ_fineTune:
        min = -99;      max = 99;
        break;
    case champ_coarseTune:
        min = -120;     max = 120;
        break;
    case champ_pan:
        min = -50;      max = 50;
        break;
    case champ_initialAttenuation: case champ_sustainVolEnv: case champ_sustainModEnv:
        min = 0;        max = 144;
        break;
    case champ_reverbEffectsSend: case champ_chorusEffectsSend:
        min = 0;        max = 100;
        break;
    case champ_scaleTuning:
        min = 0;        max = 1200;
        break;
    case champ_initialFilterFc:
        min = 18;       max = 19914;
        break;
    case champ_initialFilterQ:
        min = 0;      max = 96;
        break;
    case champ_delayVolEnv: case champ_holdVolEnv: case champ_holdModEnv:
    case champ_delayModEnv:
        min = 0.001;      max = 18;
        break;
    case champ_delayModLFO: case champ_delayVibLFO:
        min = 0.001;      max = 20;
        break;
    case champ_freqModLFO: case champ_freqVibLFO:
        min = 0.001;      max = 100;
        break;
    case champ_attackVolEnv: case champ_decayVolEnv: case champ_releaseVolEnv: case champ_releaseModEnv:
    case champ_attackModEnv: case champ_decayModEnv:
        min = 0.001;      max = 101.6;
        break;
    case champ_keynumToVolEnvHold: case champ_keynumToVolEnvDecay:
    case champ_keynumToModEnvHold: case champ_keynumToModEnvDecay:
        min = -1200;      max = 1200;
        break;
    case champ_modLfoToVolume:
        min = -96;        max = 96;
        break;
    case champ_modEnvToPitch: case champ_modEnvToFilterFc: case champ_modLfoToPitch:
    case champ_modLfoToFilterFc: case champ_vibLfoToPitch:
        min = -12000;     max = 12000;
        break;
    case champ_keynum: case champ_overridingRootKey: case champ_velocity: case champ_exclusiveClass:
        min = 0;          max = 127;
        break;
    default:
        break;
    }

    if (min != max)
    {
        if (val < min)
            return min;
        else if (val > max)
            return max;
    }

    return val;
}
