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

#include "conversion_sfz.h"
#include "pile_sf2.h"
#include <QFile>

ConversionSfz::ConversionSfz(Pile_sf2 *sf2) :
    _sf2(sf2)
{}

QString ConversionSfz::convert(QString dir, EltID id)
{
    QString path = "";
    id.typeElement = elementPrst;

    // For each preset, a sfz file is created
    for (int i = 0; i < _sf2->count(id); i++)
    {
        id.indexElt = i;
        if (!_sf2->get(id, champ_hidden).bValue)
            path = exportPrst(dir, id);
    }

    return path;
}

QString ConversionSfz::exportPrst(QString dir, EltID id)
{
    QString path = getPathSfz(dir, _sf2->getQstr(id, champ_name));
    QFile fichierSfz(path + ".sfz");
    if (fichierSfz.open(QIODevice::WriteOnly))
    {
        writeEntete(&fichierSfz, id);
        id.typeElement = elementPrstInst;
        for (int i = 0; i < _sf2->count(id); i++)
        {
            id.indexElt2 = i;
            if (!_sf2->get(id, champ_hidden).bValue)
            {
                // Paramètres du prst
                ParamListe * paramPrst = new ParamListe(_sf2, id);

                // ID de l'instrument lié
                EltID idInst = id;
                idInst.typeElement = elementInst;
                idInst.indexElt = _sf2->get(id, champ_instrument).wValue;

                // Paramètres globaux (groupe)
                ParamListe * paramGroupe = new ParamListe(_sf2, paramPrst, idInst);
                writeGroup(&fichierSfz, paramGroupe);
                delete paramGroupe;

                // Ecriture de chaque élément présent dans l'instrument
                idInst.typeElement = elementInstSmpl;
                for (int j = 0; j < _sf2->count(idInst); j++)
                {
                    idInst.indexElt2 = j;
                    if (!_sf2->get(idInst, champ_hidden).bValue)
                    {
                        ParamListe * paramInstSmpl = new ParamListe(_sf2, paramPrst, idInst);
                        writeRegion(&fichierSfz, paramInstSmpl);
                        delete paramInstSmpl;
                    }
                }

                delete paramPrst;
            }
        }
        fichierSfz.close();
    }

    return path;
}

void ConversionSfz::writeEntete(QFile * fichierSfz, EltID id)
{
    // Ecriture de l'entete
    id.typeElement = elementSf2;
    QTextStream out(fichierSfz);
    out << QObject::trUtf8("// Nom : ") << _sf2->getQstr(id, champ_name) << endl
        << QObject::trUtf8("// Auteur : ") << _sf2->getQstr(id, champ_IENG) << endl
        << QObject::trUtf8("// Copyright : ") << _sf2->getQstr(id, champ_ICOP) << endl
        << QObject::trUtf8("// Date : ") << QDate::currentDate().toString("yyyy/MM/dd") << endl
        << endl;
}

QString ConversionSfz::getPathSfz(QString dir, QString name)
{
    if (name.isEmpty())
        name = QObject::trUtf8("sans nom");
    name = name.replace(QRegExp("[\\W]"), "_");
    QFile file(dir + "/" + name + ".sfz");
    QDir dossier(dir + "/" + name);
    if (file.exists() || dossier.exists())
    {
        int i = 1;
        while (QFile(dir + "/" + name + "-" + QString::number(i) + ".sfz").exists() ||
               QDir(dir + "/" + name + "-" + QString::number(i)).exists())
            i++;
        name = name + "-" + QString::number(i);
    }
    return dir + "/" + name;
}

void ConversionSfz::writeGroup(QFile * fichierSfz, ParamListe * listeParam)
{
    // Ecriture de paramètres communs à plusieurs régions
    QTextStream out(fichierSfz);
    out << "<group>" << endl;
    for (int i = 0; i < listeParam->size(); i++)
        writeElement(out, listeParam->getChamp(i), listeParam->getValeur(i));
    out << endl;
}

void ConversionSfz::writeRegion(QFile * fichierSfz, ParamListe * listeParam)
{
    // Ecriture de paramètres spécifique à une région
    QTextStream out(fichierSfz);
    out << "<region>" << endl;
    for (int i = 0; i < listeParam->size(); i++)
        writeElement(out, listeParam->getChamp(i), listeParam->getValeur(i));
    out << endl;
}

void ConversionSfz::writeElement(QTextStream &out, Champ champ, double value)
{
    switch (champ)
    {
    case champ_fineTune:                out << "tune=" << (int)(value + 0.5) << endl;               break;
    case champ_coarseTune:              out << "transpose=" << (int)(value + 0.5) << endl;          break;
    case champ_vibLfoToPitch:           out << "pitchlfo_depth=" << (int)(value + 0.5) << endl;     break;
    case champ_scaleTuning:             out << "pitch_keytrack=" << (int)(value + 0.5) << endl;     break;
    case champ_startloopAddrsOffset:    out << "loop_start=" << (int)(value + 0.5) << endl;         break;
    case champ_startAddrsOffset:        out << "offset=" << (int)(value + 0.5) << endl;             break;
    case champ_endloopAddrsOffset:      out << "loop_end=" << (int)(value + 0.5) << endl;           break;
    case champ_endAddrsOffset:          out << "end=" << (int)(value + 0.5) << endl;                break;
    case champ_pan:                     out << "pan=" << 2 * value << endl;                         break;
    case champ_initialAttenuation:      out << "volume=" << -value << endl;                         break;
    case champ_initialFilterQ:          out << "resonance=" << value << endl;                       break;
    case champ_sustainModEnv:           out << "fileg_sustain=" << value << endl
                                            << "pitcheg_sustain=" << value << endl;                 break;
    case champ_delayModEnv:             out << "pitcheg_delay=" << value << endl
                                            << "fileg_delay=" << value << endl;                     break;
    case champ_attackModEnv:            out << "pitcheg_attack=" << value << endl
                                            << "fileg_attack=" << value << endl;                    break;
    case champ_holdModEnv:              out << "pitcheg_hold=" << value << endl
                                            << "fileg_hold=" << value << endl;                      break;
    case champ_decayModEnv:             out << "pitcheg_decay=" << value << endl
                                            << "fileg_decay=" << value << endl;                     break;
    case champ_releaseModEnv:           out << "pitcheg_release=" << value << endl
                                            << "fileg_release=" << value << endl;                   break;
    case champ_modEnvToPitch:           out << "pitcheg_depth=" << value << endl;                   break;
    case champ_modEnvToFilterFc:        out << "fileg_depth=" << value << endl;                     break;
    case champ_keynumToModEnvHold:      out << "pitcheg_vel2hold=" << value << endl
                                            << "fileg_vel2hold=" << value << endl;                  break;
    case champ_keynumToModEnvDecay:     out << "pitcheg_vel2decay=" << value << endl
                                            << "fileg_vel2decay=" << value << endl;                 break;
    case champ_delayModLFO:             out << "amplfo_delay=" << value << endl
                                            << "fillfo_delay=" << value << endl;                    break;
    case champ_freqModLFO:              out << "amplfo_freq=" << value << endl
                                            << "fillfo_freq=" << value << endl;                     break;
    case champ_modLfoToVolume:          out << "amplfo_depth=" << value << endl;                    break;
    case champ_modLfoToFilterFc:        out << "fillfo_depth=" << value << endl;                    break;

    case champ_modLfoToPitch:           /* IMPOSSIBLE !!! */                                        break;
    case champ_keynum:                  out << "pitch_keycenter=" << (int)(value + 0.5) << endl
                                            << "pitch_keytrack=0" << endl;                          break;
    case champ_reverbEffectsSend:       out << "effect1=" << value << endl;                         break;
    case champ_chorusEffectsSend:       out << "effect2=" << value << endl;                         break;
    case champ_delayVolEnv:             out << "ampeg_delay=" << value << endl;                     break;
    case champ_attackVolEnv:            out << "ampeg_attack=" << value << endl;                    break;
    case champ_sustainVolEnv:           out << "ampeg_sustain" << value << endl;                    break;
    case champ_holdVolEnv:              out << "ampeg_hold=" << value << endl;                      break;
    case champ_decayVolEnv:             out << "ampeg_decay=" << value << endl;                     break;
    case champ_keynumToVolEnvHold:      out << "ampeg_vel2hold=" << value << endl;                  break;
    case champ_keynumToVolEnvDecay:     out << "ampeg_vel2decay=" << value << endl;                 break;
    case champ_releaseVolEnv:           out << "ampeg_release=" << value << endl;                   break;
    case champ_delayVibLFO:             out << "pitchlfo_delay=" << (int)(value + 0.5) << endl;     break;
    case champ_overridingRootKey:       out << "pitch_keycenter=" << (int)(value + 0.5) << endl;    break;
    case champ_freqVibLFO:              out << "pitchlfo_freq=" << (int)(value + 0.5) << endl;      break;
    case champ_velocity:                out << "amp_velcurve_1=" << value / 127 << endl
                                            << "amp_velcurve_127=" << value / 127 << endl;          break;
    case champ_exclusiveClass:
        if (value != 0)
            out << "group=" << (int)(value + 0.5) << endl;
        break;
    case champ_initialFilterFc:
        out << "fil_type=lpf_1p" << endl
            << "cutoff="   << (int)(value + 0.5) << endl;
        break;
    case champ_keyRange:{
        int lokey = value / 1000. + 0.5;
        int hikey = value - 1000. * lokey + 0.5;
        out << "lokey=" << lokey << " hikey=" << hikey << endl;
        }break;
    case champ_velRange:{
        int lovel = value / 1000. + 0.5;
        int hivel = value - 1000. * lovel + 0.5;
        out << "lovel=" << lovel << " hivel=" << hivel << endl;
        }break;
    case champ_sampleModes:
        if (value == 0.)
            out << "loop_mode=no_loop" << endl;
        else if (value == 1.)
            out << "loop_mode=loop_continuous" << endl;
        else if (value == 3.)
            out << "loop_mode=loop_sustain" << endl;
        break;
    default:
        break;
    }
}


ParamListe::ParamListe(Pile_sf2 * sf2, EltID id)
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

ParamListe::ParamListe(Pile_sf2 * sf2, ParamListe * paramPrst, EltID idInst)
{
    if (idInst.typeElement == elementInst)
    {
        // Chargement des paramètres globaux d'un instrument
        idInst.typeElement = elementInstGen;
        load(sf2, idInst);
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

        if (_listeChamps.indexOf(champ_overridingRootKey) == -1)
        {
            _listeChamps << champ_overridingRootKey;
            _listeValeurs << sf2->get(idSmpl, champ_byOriginalPitch).bValue;
        }

        if (_listeChamps.indexOf(champ_fineTune) == -1)
        {
            _listeChamps << champ_fineTune;
            _listeValeurs << sf2->get(idSmpl, champ_chPitchCorrection).cValue;
        }
        else
        {
            _listeValeurs[_listeChamps.indexOf(champ_fineTune)] +=
                    sf2->get(idSmpl, champ_chPitchCorrection).cValue;
        }

        if (_listeChamps.indexOf(champ_startloopAddrsOffset) == -1)
        {
            _listeChamps << champ_startloopAddrsOffset;
            _listeValeurs << sf2->get(idSmpl, champ_dwStartLoop).dwValue;
        }
        else
        {
            _listeValeurs[_listeChamps.indexOf(champ_startloopAddrsOffset)] +=
                    sf2->get(idSmpl, champ_dwStartLoop).dwValue;
        }

        if (_listeChamps.indexOf(champ_endloopAddrsOffset) == -1)
        {
            _listeChamps << champ_endloopAddrsOffset;
            _listeValeurs << sf2->get(idSmpl, champ_dwEndLoop).dwValue;
        }
        else
        {
            _listeValeurs[_listeChamps.indexOf(champ_endloopAddrsOffset)] +=
                    sf2->get(idSmpl, champ_dwEndLoop).dwValue;
        }

        if (_listeChamps.indexOf(champ_endAddrsOffset) == -1)
        {
            _listeChamps << champ_endAddrsOffset;
            _listeValeurs << sf2->get(idSmpl, champ_dwLength).dwValue;
        }
        else
        {
            _listeValeurs[_listeChamps.indexOf(champ_endAddrsOffset)] +=
                    sf2->get(idSmpl, champ_dwLength).dwValue;
        }
    }

    // Gestion des offsets
    mix(champ_startAddrsCoarseOffset, champ_startAddrsOffset);
    mix(champ_endAddrsCoarseOffset, champ_endAddrsOffset);
    mix(champ_startloopAddrsCoarseOffset, champ_startloopAddrsOffset);
    mix(champ_endloopAddrsCoarseOffset, champ_endloopAddrsOffset);

    // Gestion de la note fixe
    if (_listeChamps.indexOf(champ_keynum) != -1)
        _listeChamps.removeAll(champ_overridingRootKey);

    // Fusion des 2 listes de paramètres
    if (idInst.typeElement == elementInstGen)
    {
        for (int i = 0; i < paramPrst->_listeChamps.size(); i++)
            fusion(_listeChamps.at(i), _listeValeurs.at(i));
    }
    else if (idInst.typeElement == elementInstSmplGen)
    {
        // On fusionne uniquement avec les éléments présents
        for (int i = 0; i < paramPrst->_listeChamps.size(); i++)
        {
            if (_listeChamps.indexOf(paramPrst->_listeChamps.at(i)) != -1)
                fusion(_listeChamps.at(i), _listeValeurs.at(i));
        }
    }

    // Limites
    for (int i = 0; i < _listeChamps.size(); i++)
        _listeValeurs[i] = limit(_listeValeurs.at(i), _listeChamps.at(i));

    // Ordre
    prepend(champ_velRange);
    prepend(champ_keynum);
    prepend(champ_overridingRootKey);
    prepend(champ_keyRange);
}

void ParamListe::prepend(Champ champ)
{
    if (_listeChamps.indexOf(champ) != -1)
    {
        int index = _listeChamps.indexOf(champ);
        Champ chTmp = _listeChamps.takeAt(index);
        double value = _listeValeurs.takeAt(index);
        _listeChamps.prepend(chTmp);
        _listeValeurs.prepend(value);
    }
}

void ParamListe::load(Pile_sf2 * sf2, EltID id)
{
    // Charge les paramètres, n'écrase pas les valeurs existantes
    bool isPrst = (id.typeElement == elementPrstInstGen || id.typeElement == elementPrstGen);
    for (int i = 0; i < sf2->count(id); i++)
    {
        id.indexMod = i;
        if (_listeChamps.indexOf(sf2->get(id, champ_sfGenOper).sfGenValue) == -1)
        {
            Champ champ = sf2->get(id, champ_sfGenOper).sfGenValue;
            _listeChamps << champ;
            _listeValeurs << getValue(champ, sf2->get(id, champ_sfGenAmount).genValue, isPrst);
        }
    }
}

void ParamListe::mix(Champ champCoarse, Champ champFine)
{
    if (_listeChamps.indexOf(champCoarse) != -1)
    {
        int indexCoarse = _listeChamps.indexOf(champCoarse);
        if (_listeChamps.indexOf(champFine) != -1)
        {
            _listeValeurs[_listeChamps.indexOf(champFine)] += _listeValeurs[indexCoarse];
            _listeChamps.removeAt(indexCoarse);
            _listeValeurs.removeAt(indexCoarse);
        }
        else
            _listeChamps[indexCoarse] = champFine;
    }
}

double ParamListe::getDefaultValue(Champ champ)
{
    double defValue;
    switch (champ)
    {
    case champ_fineTune: case champ_coarseTune: case champ_initialFilterQ:
    case champ_startAddrsOffset: case champ_endAddrsOffset: case champ_startloopAddrsOffset:
    case champ_endloopAddrsOffset: case champ_initialAttenuation: case champ_pan:
    case champ_keynumToVolEnvHold: case champ_keynumToVolEnvDecay:
    case champ_sustainVolEnv: case champ_keynumToModEnvHold: case champ_keynumToModEnvDecay:
    case champ_sustainModEnv: case champ_modEnvToPitch: case champ_modEnvToFilterFc:
    case champ_modLfoToPitch: case champ_modLfoToFilterFc: case champ_modLfoToVolume:
    case champ_vibLfoToPitch: case champ_reverbEffectsSend: case champ_chorusEffectsSend:
    case champ_exclusiveClass: case champ_sampleModes:
        defValue = 0;
        break;
    case champ_keynum: case champ_overridingRootKey: case champ_velocity:
        defValue = -1;
        break;
    case champ_initialFilterFc:
        defValue = 19914;
        break;
    case champ_delayVolEnv: case champ_attackVolEnv: case champ_holdVolEnv:
    case champ_decayVolEnv: case champ_releaseVolEnv: case champ_delayModEnv:
    case champ_attackModEnv: case champ_holdModEnv: case champ_decayModEnv:
    case champ_releaseModEnv: case champ_delayModLFO: case champ_delayVibLFO:
        defValue = 0.001;
        break;
    case champ_freqModLFO: case champ_freqVibLFO:
        defValue = 8.176;
        break;
    case champ_scaleTuning:
        defValue = 100;
        break;
    case champ_keyRange: case champ_velRange:
        defValue = 127;
        break;
    default:
        defValue = 0;
        break;
    }
    return defValue;
}

void ParamListe::fusion(Champ champ, double value)
{
    int index = _listeChamps.indexOf(champ);
    if (index == -1)
    {
        index = _listeChamps.size();
        _listeChamps.append(champ);
        _listeValeurs.append(getDefaultValue(champ));
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
        _listeValeurs[index] += _listeValeurs[index];
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
        int minNew = value / 1000 + 0.5;
        int maxNew = value - 1000 * minNew + 0.5;
        int minOld = _listeValeurs.at(index) / 1000 + 0.5;
        int maxOld = _listeValeurs.at(index) - 1000 * minOld + 0.5;
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

double ParamListe::getValue(Champ champ, genAmountType amount, bool isPrst)
{
    double dRet = 0;
    switch (champ)
    {
    case champ_fineTune: case champ_coarseTune: case champ_keynumToVolEnvHold: case champ_keynumToVolEnvDecay:
    case champ_keynumToModEnvHold: case champ_keynumToModEnvDecay: case champ_modEnvToPitch:
    case champ_modEnvToFilterFc: case champ_modLfoToPitch: case champ_modLfoToFilterFc:
    case champ_vibLfoToPitch: case champ_scaleTuning:
    case champ_startloopAddrsOffset: case champ_startAddrsOffset: case champ_endloopAddrsOffset:
    case champ_endAddrsOffset:
        dRet = amount.shAmount;
        break;
    case champ_pan: case champ_initialAttenuation: case champ_initialFilterQ: case champ_sustainVolEnv:
    case champ_sustainModEnv: case champ_modLfoToVolume: case champ_reverbEffectsSend:
    case champ_chorusEffectsSend:
        dRet = (double)amount.shAmount / 10.;
        break;
    case champ_delayVolEnv: case champ_attackVolEnv: case champ_holdVolEnv: case champ_decayVolEnv:
    case champ_releaseVolEnv: case champ_delayModEnv: case champ_attackModEnv: case champ_holdModEnv:
    case champ_decayModEnv: case champ_releaseModEnv: case champ_delayModLFO: case champ_delayVibLFO:
        dRet = d1200e2(amount.shAmount);
        break;
    case champ_overridingRootKey: case champ_keynum: case champ_velocity: case champ_sampleModes:
    case champ_exclusiveClass: case champ_sampleID:
        dRet = amount.wAmount;
        break;
    case champ_startloopAddrsCoarseOffset: case champ_endloopAddrsCoarseOffset:
    case champ_startAddrsCoarseOffset: case champ_endAddrsCoarseOffset:
        dRet = amount.shAmount * 32768;
        break;
    case champ_initialFilterFc: case champ_freqModLFO: case champ_freqVibLFO:
        if (isPrst)
            dRet = d1200e2(amount.shAmount);
        else
            dRet = d1200e2(amount.shAmount) * 8.176;
        break;
    case champ_keyRange: case champ_velRange:
        dRet = amount.ranges.byLo * 1000 + amount.ranges.byHi;
        break;
    default:
        break;
    }
    return dRet;
}

double ParamListe::limit(double val, Champ champ)
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
        min = 0;      max = 144;
        break;
    case champ_reverbEffectsSend: case champ_chorusEffectsSend:
        min = 0;      max = 100;
        break;
    case champ_scaleTuning:
        min = 0;      max = 1200;
        break;
    case champ_initialFilterFc:
        min = 18;      max = 19914;
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
