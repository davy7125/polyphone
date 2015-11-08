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

#include "conversion_sfz.h"
#include "pile_sf2.h"
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDate>
#include "config.h"

ConversionSfz::ConversionSfz(Pile_sf2 *sf2) :
    _sf2(sf2)
{}

void ConversionSfz::convert(QString dir, QList<EltID> listID, bool presetPrefix, bool bankDir, bool gmSort)
{
    if (listID.isEmpty())
        return;

    // Création d'un répertoire
    EltID idSF2 = listID.at(0);
    idSF2.typeElement = elementSf2;
    QString rootDir = QFileInfo(_sf2->getQstr(idSF2, champ_filename)).baseName();
    if (rootDir.isEmpty())
        rootDir = escapeStr(_sf2->getQstr(idSF2, champ_name));
    QDir dossier(dir + "/" + rootDir);
    if (dossier.exists())
    {
        int i = 1;
        while (QDir(dir + "/" + rootDir + "-" + QString::number(i)).exists())
            i++;
        rootDir += "-" + QString::number(i);
    }
    rootDir = dir + "/" + rootDir;

    if (!QDir(rootDir).mkdir(rootDir))
        return;

    // Plusieurs banques sont utilisées ?
    int numBankUnique = -1;
    _bankSortEnabled = false;
    for (int i = 0; i < listID.count(); i++)
    {
        if (numBankUnique == -1)
            numBankUnique = _sf2->get(listID.at(i), champ_wBank).wValue;
        else
            _bankSortEnabled |= (numBankUnique != _sf2->get(listID.at(i), champ_wBank).wValue);
    }
    if (_bankSortEnabled)
        numBankUnique = -1; // Si numBankUnique est différent de -1, il nous donne le numéro unique de banque utilisée
    _bankSortEnabled &= bankDir;
    _gmSortEnabled = gmSort;

    // Répertoire samples
    _dirSamples = rootDir + "/samples";
    QDir().mkdir(_dirSamples);


    for (int i = 0; i < listID.count(); i++)
    {
        // Répertoire allant contenir le fichier sfz
        QString sourceDir = rootDir;

        int numBank = _sf2->get(listID.at(i), champ_wBank).wValue;
        int numPreset = _sf2->get(listID.at(i), champ_wPreset).wValue;

        if (_bankSortEnabled)
        {
            QString numText;
            numText.sprintf("%.3u", numBank);
            sourceDir += "/" + numText;
            if (!QDir(sourceDir).exists())
                QDir(sourceDir).mkdir(sourceDir);
        }
        if (_gmSortEnabled)
        {
            if (numBank == 128)
            {
                if (_bankSortEnabled || numBankUnique == 128)
                    sourceDir += "/" + getDrumCategory(numPreset);
                else
                    sourceDir += "/" + getDirectoryName(128);
            }
            else
                sourceDir += "/" + getDirectoryName(numPreset);
            if (!QDir(sourceDir).exists())
                QDir(sourceDir).mkdir(sourceDir);
        }

        exportPrst(sourceDir, listID.at(i), presetPrefix);
    }
}

void ConversionSfz::exportPrst(QString dir, EltID id, bool presetPrefix)
{
    QString numText;
    if (presetPrefix)
        numText.sprintf("%.3u_", _sf2->get(id, champ_wPreset).wValue);
    int numBank = _sf2->get(id, champ_wBank).wValue;

    QFile fichierSfz(getPathSfz(dir, numText + _sf2->getQstr(id, champ_name)) + ".sfz");
    if (fichierSfz.open(QIODevice::WriteOnly))
    {
        writeEntete(&fichierSfz, id);
        id.typeElement = elementPrstInst;

        for (int i = 0; i < _sf2->count(id); i++)
        {
            id.indexElt2 = i;
            if (!_sf2->get(id, champ_hidden).bValue)
            {
                QList<EltID> listProcessedInstSmpl;

                // Paramètres du prst
                ParamListe * paramPrst = new ParamListe(_sf2, id);

                // ID de l'instrument lié
                EltID idInst = id;
                idInst.typeElement = elementInst;
                idInst.indexElt = _sf2->get(id, champ_instrument).wValue;

                // Paramètres globaux (groupe)
                ParamListe * paramGroupe = new ParamListe(_sf2, paramPrst, idInst);
                writeGroup(&fichierSfz, paramGroupe, numBank == 128);
                delete paramGroupe;

                // Ecriture de chaque élément présent dans l'instrument
                idInst.typeElement = elementInstSmpl;

                // Ordre des instSmpl
                QMultiMap<int, EltID> map;
                for (int j = 0; j < _sf2->count(idInst); j++)
                {
                    idInst.indexElt2 = j;
                    if (!_sf2->get(idInst, champ_hidden).bValue)
                        map.insert(_sf2->get(idInst, champ_keyRange).rValue.byLo, idInst);
                }

                QList<EltID> listInstSmpl = map.values();
                for (int j = 0; j < listInstSmpl.count(); j++)
                {
                    idInst = listInstSmpl.at(j);
                    if (!listProcessedInstSmpl.contains(idInst) && isIncluded(paramPrst, idInst))
                    {
                        ParamListe * paramInstSmpl = new ParamListe(_sf2, paramPrst, idInst);
                        EltID idSmpl = idInst;
                        idSmpl.typeElement = elementSmpl;
                        idSmpl.indexElt = _sf2->get(idInst, champ_sampleID).wValue;

                        // Utilisation sample mono ou stéréo ?
                        SFSampleLink typeSample = _sf2->get(idSmpl, champ_sfSampleType).sfLinkValue;
                        double pan = 0;
                        if (paramInstSmpl->findChamp(champ_pan) != -1)
                            pan = paramInstSmpl->getValeur(paramInstSmpl->findChamp(champ_pan));
                        bool enableStereo = ((typeSample == leftSample || typeSample == RomLeftSample) && pan == -50) ||
                                ((typeSample == rightSample || typeSample == RomRightSample) && pan == 50);

                        bool ignorePan = false;
                        if (enableStereo)
                        {
                            // Sample lié
                            EltID idSmplLinked = idSmpl;
                            idSmplLinked.indexElt = _sf2->get(idSmpl, champ_wSampleLink).wValue;

                            // Vérification que les paramètres liés aux samples sont identiques
                            if (_sf2->get(idSmpl, champ_dwStartLoop).dwValue == _sf2->get(idSmplLinked, champ_dwStartLoop).dwValue &&
                                _sf2->get(idSmpl, champ_dwEndLoop).dwValue == _sf2->get(idSmplLinked, champ_dwEndLoop).dwValue &&
                                _sf2->get(idSmpl, champ_dwLength).dwValue == _sf2->get(idSmplLinked, champ_dwLength).dwValue &&
                                _sf2->get(idSmpl, champ_chPitchCorrection).dwValue == _sf2->get(idSmplLinked, champ_chPitchCorrection).dwValue &&
                                _sf2->get(idSmpl, champ_byOriginalPitch).dwValue == _sf2->get(idSmplLinked, champ_byOriginalPitch).dwValue &&
                                _sf2->get(idSmpl, champ_dwSampleRate).dwValue == _sf2->get(idSmplLinked, champ_dwSampleRate).dwValue)
                            {
                                // Recherche d'un instSmpl correspondant exactement à l'autre canal
                                int index = -1;
                                for (int k = j + 1; k < listInstSmpl.count(); k++)
                                {
                                    if (_sf2->get(listInstSmpl.at(k), champ_sampleID).wValue == idSmplLinked.indexElt)
                                    {
                                        // Comparaison des gen
                                        bool isEqual = true;
                                        EltID idInstSmplGen = idInst;
                                        idInstSmplGen.typeElement = elementInstSmplGen;
                                        EltID idLinkedInstSmplGen = listInstSmpl.at(k);
                                        idLinkedInstSmplGen.typeElement = elementInstSmplGen;
                                        QMap<Champ, genAmountType> map, mapLinked;
                                        for (int l = 0; l < _sf2->count(idInstSmplGen); l++)
                                        {
                                            idInstSmplGen.indexMod = l;
                                            map.insert(_sf2->get(idInstSmplGen, champ_sfGenOper).sfGenValue,
                                                       _sf2->get(idInstSmplGen, champ_sfGenAmount).genValue);
                                        }
                                        for (int l = 0; l < _sf2->count(idLinkedInstSmplGen); l++)
                                        {
                                            idLinkedInstSmplGen.indexMod = l;
                                            mapLinked.insert(_sf2->get(idLinkedInstSmplGen, champ_sfGenOper).sfGenValue,
                                                       _sf2->get(idLinkedInstSmplGen, champ_sfGenAmount).genValue);
                                        }
                                        QList<Champ> listeChamps = map.keys();
                                        QList<Champ> listeChampsLinked = mapLinked.keys();
                                        QList<genAmountType> listeValeurs = map.values();
                                        QList<genAmountType> listeValeursLinked = mapLinked.values();
                                        if (listeChamps.size() == listeChampsLinked.size())
                                        {
                                            for (int l = 0; l < listeChamps.size(); l++)
                                            {
                                                if (listeChamps.at(l) == champ_pan)
                                                    isEqual &= listeChamps.at(l) == listeChampsLinked.at(l) &&
                                                        listeValeurs.at(l).shAmount == -listeValeursLinked.at(l).shAmount;
                                                else if (listeChamps.at(l) != champ_sampleID)
                                                    isEqual &= listeChamps.at(l) == listeChampsLinked.at(l) &&
                                                        listeValeurs.at(l).shAmount == listeValeursLinked.at(l).shAmount;
                                            }
                                        }
                                        else
                                            isEqual = false;
                                        if (isEqual)
                                            index = k;
                                    }
                                }
                                if (index != -1)
                                {
                                    listProcessedInstSmpl << listInstSmpl.at(index);
                                    ignorePan = true;
                                }
                            }
                            else
                                enableStereo = false;
                        }
                        writeRegion(&fichierSfz, paramInstSmpl, getLink(idSmpl, enableStereo), ignorePan);
                        delete paramInstSmpl;
                        listProcessedInstSmpl << idInst;
                    }
                }
                delete paramPrst;
            }
        }
        fichierSfz.close();
    }

    return;
}

QString ConversionSfz::getPathSfz(QString dir, QString name)
{
    if (name.isEmpty())
        name = QObject::trUtf8("sans nom");
    name = escapeStr(name);
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

void ConversionSfz::writeEntete(QFile * fichierSfz, EltID id)
{
    // Ecriture de l'entete
    id.typeElement = elementSf2;
    QTextStream out(fichierSfz);
    out << QObject::trUtf8("// Sfz exporté depuis un sf2 par Polyphone") << endl
        << QObject::trUtf8("// Nom         : ") << _sf2->getQstr(id, champ_name).replace(QRegExp("[\r\n]"), " ") << endl
        << QObject::trUtf8("// Auteur      : ") << _sf2->getQstr(id, champ_IENG).replace(QRegExp("[\r\n]"), " ") << endl
        << QObject::trUtf8("// Copyright   : ") << _sf2->getQstr(id, champ_ICOP).replace(QRegExp("[\r\n]"), " ") << endl
        << QObject::trUtf8("// Date        : ") << QDate::currentDate().toString("yyyy/MM/dd") << endl
        << QObject::trUtf8("// Commentaire : ") << _sf2->getQstr(id, champ_ICMT).replace(QRegExp("[\r\n]"), " ") << endl;
}

void ConversionSfz::writeGroup(QFile * fichierSfz, ParamListe * listeParam, bool isPercKit)
{
    // Ecriture de paramètres communs à plusieurs régions
    QTextStream out(fichierSfz);
    out << endl << "<group>" << endl;
    if (isPercKit)
        out << "lochan=10 hichan=10" << endl;
    for (int i = 0; i < listeParam->size(); i++)
        writeElement(out, listeParam->getChamp(i), listeParam->getValeur(i));
}

void ConversionSfz::writeRegion(QFile * fichierSfz, ParamListe * listeParam, QString pathSample, bool ignorePan)
{
    // Correction de volume lorsqu'un son passe en stéréo
    double deltaVolumeIfIgnorePan = 3.;

    // Ecriture de paramètres spécifique à une région
    QTextStream out(fichierSfz);

    out << endl << "<region>" << endl
        << "sample=" << pathSample.replace("/", "\\") << endl;
    if (ignorePan && listeParam->findChamp(champ_initialAttenuation) == -1)
        writeElement(out, champ_initialAttenuation, -deltaVolumeIfIgnorePan / DB_SF2_TO_SFZ);

    for (int i = 0; i < listeParam->size(); i++)
    {
        if (ignorePan && listeParam->getChamp(i) == champ_initialAttenuation)
            writeElement(out, champ_initialAttenuation, listeParam->getValeur(i) -
                         deltaVolumeIfIgnorePan / DB_SF2_TO_SFZ);
        else if (!ignorePan || listeParam->getChamp(i) != champ_pan)
            writeElement(out, listeParam->getChamp(i), listeParam->getValeur(i));
    }
}

void ConversionSfz::writeElement(QTextStream &out, Champ champ, double value)
{
    QString v2 = " // sfz v2";
    switch (champ)
    {
    case champ_fineTune:                out << "tune=" << qRound(value) << endl;                    break;
    case champ_coarseTune:              out << "transpose=" << qRound(value) << endl;               break;
    case champ_scaleTuning:             out << "pitch_keytrack=" << qRound(value) << endl;          break;
    case champ_startloopAddrsOffset:    out << "loop_start=" << qRound(value) << endl;              break;
    case champ_startAddrsOffset:        out << "offset=" << qRound(value) << endl;                  break;
    case champ_endloopAddrsOffset:      out << "loop_end=" << qRound(value) - 1 << endl;            break;
    case champ_endAddrsOffset:          out << "end=" << qRound(value) - 1 << endl;                 break;
    case champ_pan:                     out << "pan=" << 2 * value << endl;                         break;
    case champ_initialAttenuation:      out << "volume=" << -value * DB_SF2_TO_SFZ << endl;         break;
    case champ_initialFilterQ:          out << "resonance=" << value << endl;                       break;
    case champ_sustainModEnv:           out << "fileg_sustain=" << 100. - value << endl
                                            << "pitcheg_sustain=" << 100. - value << endl;          break;
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
    case champ_modEnvToPitch:           out << "pitcheg_depth=" << qRound(value) << endl;           break;
    case champ_modEnvToFilterFc:        out << "fileg_depth=" << qRound(value) << endl;             break;
    case champ_keynumToModEnvHold:      out << "pitcheg_holdcc133=" << value << v2 << endl
                                            << "fileg_holdcc133=" << value << v2 << endl;           break;
    case champ_keynumToModEnvDecay:     out << "pitcheg_decaycc133=" << value << v2 << endl
                                            << "fileg_decaycc133=" << value << v2 << endl;          break;
    case champ_delayModLFO:             out << "amplfo_delay=" << value << endl
                                            << "fillfo_delay=" << value << endl;                    break;
    case champ_freqModLFO:              out << "amplfo_freq=" << value << endl
                                            << "fillfo_freq=" << value << endl;                     break;
    case champ_modLfoToVolume:          out << "amplfo_depth=" << value << endl;                    break;
    case champ_modLfoToFilterFc:        out << "fillfo_depth=" << value << endl;                    break;

    case champ_modLfoToPitch:           /* IMPOSSIBLE !!! */                                        break;
    case champ_keynum:
        out << "pitch_keycenter="
            << Config::getInstance()->getKeyName(qRound(value), false, false, true) << endl
            << "pitch_keytrack=0" << endl;
        break;
    case champ_reverbEffectsSend:       out << "effect1=" << value << endl;                         break;
    case champ_chorusEffectsSend:       out << "effect2=" << value << endl;                         break;
    case champ_delayVolEnv:             out << "ampeg_delay=" << value << endl;                     break;
    case champ_attackVolEnv:            out << "ampeg_attack=" << value << endl;                    break;
    case champ_sustainVolEnv:           out << "ampeg_sustain=" << dbToPercent(value) << endl;      break;
    case champ_holdVolEnv:              out << "ampeg_hold=" << value << endl;                      break;
    case champ_decayVolEnv:             out << "ampeg_decay=" << value << endl;                     break;
    case champ_keynumToVolEnvHold:      out << "ampeg_holdcc133=" << value << v2 << endl;           break;
    case champ_keynumToVolEnvDecay:     out << "ampeg_decaycc133=" << value << v2 << endl;          break;
    case champ_releaseVolEnv:           out << "ampeg_release=" << value << endl;                   break;
    case champ_overridingRootKey:
        out << "pitch_keycenter="
            << Config::getInstance()->getKeyName(qRound(value), false, false, true) << endl;
        break;
    case champ_delayVibLFO:             out << "pitchlfo_delay=" << value << endl;                  break;
    case champ_freqVibLFO:              out << "pitchlfo_freq=" << value << endl;                   break;
    case champ_vibLfoToPitch:           out << "pitchlfo_depth=" << qRound(value) << endl;          break;
    case champ_velocity:                out << "amp_velcurve_1=" << value / 127. << endl
                                            << "amp_velcurve_127=" << value / 127. << endl;         break;
    case champ_exclusiveClass:
        if (value != 0)
            out << "group=" << qRound(value) << endl
                << "off_by=" << qRound(value) << endl;
        break;
    case champ_initialFilterFc:
        out << "fil_type=lpf_2p" << endl
            << "cutoff="   << qRound(value) << endl;
        break;
    case champ_keyRange:{
        QString lokey = Config::getInstance()->getKeyName(qRound(value / 1000.), false, false, true);
        QString hikey = Config::getInstance()->getKeyName(qRound(value - 1000. * qRound(value / 1000.)), false, false, true);
        out << "lokey=" << lokey << " hikey=" << hikey << endl;
        }break;
    case champ_velRange:{
        int lovel = qRound(value / 1000.);
        int hivel = qRound(value - 1000. * lovel);
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

QString ConversionSfz::getLink(EltID idSmpl, bool enableStereo)
{
    QString path = "";
    if (!enableStereo && _mapMonoSamples.contains(idSmpl.indexElt))
        path = _mapMonoSamples.value(idSmpl.indexElt);
    else if (enableStereo && _mapStereoSamples.contains(idSmpl.indexElt))
        path = _mapStereoSamples.value(idSmpl.indexElt);
    else
    {
        QString name;
        EltID idSmpl2 = idSmpl;
        idSmpl2.indexElt = -1;

        // Stéréo ?
        if (enableStereo && _sf2->get(idSmpl, champ_sfSampleType).wValue != monoSample &&
                _sf2->get(idSmpl, champ_sfSampleType).wValue != RomMonoSample)
        {
            idSmpl2.indexElt = _sf2->get(idSmpl, champ_wSampleLink).wValue;

            // Nom du fichier
            QString nom1 = _sf2->getQstr(idSmpl, champ_name);
            QString nom2 = _sf2->getQstr(idSmpl2, champ_name);
            int nb = Sound::lastLettersToRemove(nom1, nom2);
            name = nom1.left(nom1.size() - nb);

            if (_sf2->get(idSmpl, champ_sfSampleType).wValue != rightSample &&
                    _sf2->get(idSmpl, champ_sfSampleType).wValue != RomRightSample)
            {
                // Inversion smpl1 smpl2
                EltID idTmp = idSmpl;
                idSmpl = idSmpl2;
                idSmpl2 = idTmp;
            }
        }
        else
            name = _sf2->getQstr(idSmpl, champ_name);

        name = escapeStr(name);
        QFile file(_dirSamples + "/" + name + ".wav");
        if (file.exists())
        {
            int i = 1;
            while (QFile(_dirSamples + "/" + name + "-" + QString::number(i) + ".wav").exists())
                i++;
            name = name + "-" + QString::number(i);
        }

        path = QDir(_dirSamples).dirName() + "/" + name + ".wav";
        if (_bankSortEnabled)
            path.prepend("../");
        if (_gmSortEnabled)
            path.prepend("../");

        // Export et sauvegarde
        if (idSmpl.indexElt == -1 || idSmpl2.indexElt == -1)
        {
            // Mono
            if (idSmpl.indexElt2 != -1)
            {
                _mapMonoSamples.insert(idSmpl.indexElt, path);
                Sound::exporter(_dirSamples + "/" + name + ".wav", _sf2->getSon(idSmpl));
            }
            else if (idSmpl2.indexElt != -1)
            {
                _mapMonoSamples.insert(idSmpl2.indexElt, path);
                Sound::exporter(_dirSamples + "/" + name + ".wav", _sf2->getSon(idSmpl2));
            }
        }
        else
        {
            // Stéréo
            _mapStereoSamples.insert(idSmpl.indexElt, path);
            _mapStereoSamples.insert(idSmpl2.indexElt, path);
            Sound::exporter(_dirSamples + "/" + name + ".wav", _sf2->getSon(idSmpl2), _sf2->getSon(idSmpl));
        }
    }
    return path;
}

QString ConversionSfz::escapeStr(QString str)
{
    return str.replace(QRegExp(QString::fromUtf8("[`~*|:<>«»?/{}\"\\\\]")), "_");
}

int ConversionSfz::lastLettersToRemove(QString str1, QString str2)
{
    str1 = str1.toLower();
    str2 = str2.toLower();
    int nbLetters = 0;

    int size = 0;
    if (str1.size() == str2.size())
        size = str1.size();
    else return 0;

    if (str1.left(size - 2).compare(str2.left(size - 2)) != 0)
        return 0;

    QString fin1_3 = str1.right(3);
    QString fin2_3 = str2.right(3);
    QString fin1_2 = str1.right(2).left(1);
    QString fin2_2 = str2.right(2).left(1);
    QString fin1_1 = str1.right(1);
    QString fin2_1 = str2.right(1);

    if ((fin1_3.compare("(r)") == 0 && fin2_3.compare("(l)") == 0) ||
        (fin1_3.compare("(l)") == 0 && fin2_3.compare("(r)") == 0))
        nbLetters = 3;
    else if (((fin1_1.compare("r") == 0 && fin2_1.compare("l") == 0) ||
              (fin1_1.compare("l") == 0 && fin2_1.compare("r") == 0)) &&
             str1.left(size - 1).compare(str2.left(size - 1)) == 0)
    {
        nbLetters = 1;
        if ((fin1_2.compare("-") == 0 && fin2_2.compare("-") == 0) ||
            (fin1_2.compare("_") == 0 && fin2_2.compare("_") == 0) ||
            (fin1_2.compare(".") == 0 && fin2_2.compare(".") == 0))
            nbLetters = 2;
    }

    return nbLetters;
}

QString ConversionSfz::getDirectoryName(int numPreset)
{
    QString strRet = QObject::trUtf8("autre");

    if (numPreset < 8)
        strRet = "000-007 " + QObject::trUtf8("Piano");
    else if (numPreset < 16)
        strRet = "008-015 " + QObject::trUtf8("Percussions chromatiques");
    else if (numPreset < 24)
        strRet = "016-023 " + QObject::trUtf8("Orgues");
    else if (numPreset < 32)
        strRet = "024-031 " + QObject::trUtf8("Guitares");
    else if (numPreset < 40)
        strRet = "032-039 " + QObject::trUtf8("Basses");
    else if (numPreset < 48)
        strRet = "040-047 " + QObject::trUtf8("Cordes");
    else if (numPreset < 56)
        strRet = "048-055 " + QObject::trUtf8("Orchestre");
    else if (numPreset < 64)
        strRet = "056-063 " + QObject::trUtf8("Cuivres");
    else if (numPreset < 72)
        strRet = "064-071 " + QObject::trUtf8("Instrument à anches");
    else if (numPreset < 80)
        strRet = "072-079 " + QObject::trUtf8("Flûtes");
    else if (numPreset < 88)
        strRet = "080-087 " + QObject::trUtf8("Synthétiseur solo");
    else if (numPreset < 96)
        strRet = "088-095 " + QObject::trUtf8("Nappes de synthétiseur");
    else if (numPreset < 104)
        strRet = "096-103 " + QObject::trUtf8("Effets de synthétiseur");
    else if (numPreset < 112)
        strRet = "104-111 " + QObject::trUtf8("Instruments ethniques");
    else if (numPreset < 120)
        strRet = "112-119 " + QObject::trUtf8("Percussions");
    else if (numPreset < 128)
        strRet = "120-127 " + QObject::trUtf8("Effets sonores");
    else if (numPreset == 128)
        strRet = QObject::trUtf8("Kits de percussion");

    return strRet;
}

QString ConversionSfz::getDrumCategory(int numPreset)
{
    QString strRet = QObject::trUtf8("autre");

    if (numPreset < 8)
        strRet = "000-007 Standard kit";
    else if (numPreset < 16)
        strRet = "008-015 Room kit";
    else if (numPreset < 24)
        strRet = "016-023 Power kit";
    else if (numPreset < 32)
        strRet = "024-031 Electronic kit";
    else if (numPreset < 40)
        strRet = "032-039 Jazz kit";
    else if (numPreset < 48)
        strRet = "040-047 Brush kit";
    else if (numPreset < 56)
        strRet = "048-055 Orchestra kit";
    else if (numPreset < 64)
        strRet = "056-063 Sound FX kit";
    else if (numPreset < 72)
        strRet = "064-071 Additional kit";
    else if (numPreset < 80)
        strRet = "072-079 Additional kit";
    else if (numPreset < 88)
        strRet = "080-087 Additional kit";
    else if (numPreset < 96)
        strRet = "088-095 Additional kit";
    else if (numPreset < 104)
        strRet = "096-103 Additional kit";
    else if (numPreset < 112)
        strRet = "104-111 Additional kit";
    else if (numPreset < 120)
        strRet = "112-119 Additional kit";
    else if (numPreset < 128)
        strRet = "120-127 Additional kit";

    return strRet;
}


bool ConversionSfz::isIncluded(ParamListe * paramPrst, EltID idInstSmpl)
{
    // Etendues de la division globale
    int prstMinKey = 0, prstMaxKey = 127, prstMinVel = 0, prstMaxVel = 127;
    int index = paramPrst->findChamp(champ_keyRange);
    if (index != -1)
    {
        prstMinKey = qRound(paramPrst->getValeur(index) / 1000);
        prstMaxKey = qRound(paramPrst->getValeur(index) - prstMinKey * 1000);
    }
    index = paramPrst->findChamp(champ_velRange);
    if (index != -1)
    {
        prstMinVel = qRound(paramPrst->getValeur(index) / 1000);
        prstMaxVel = qRound(paramPrst->getValeur(index) - prstMinVel * 1000);
    }

    // Etendues de la division de l'instrument
    int instMinKey = 0, instMaxKey = 127, instMinVel = 0, instMaxVel = 127;
    if (_sf2->isSet(idInstSmpl, champ_keyRange))
    {
        rangesType range = _sf2->get(idInstSmpl, champ_keyRange).rValue;
        instMinKey = range.byLo;
        instMaxKey = range.byHi;
    }
    if (_sf2->isSet(idInstSmpl, champ_velRange))
    {
        rangesType range = _sf2->get(idInstSmpl, champ_velRange).rValue;
        instMinVel = range.byLo;
        instMaxVel = range.byHi;
    }

    return instMinKey <= prstMaxKey && instMaxKey >= prstMinKey &&
            instMinVel <= prstMaxVel && instMaxVel >= prstMinVel;
}


int ParamListe::_boucleGlobale = -2;

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

        idInst.typeElement = elementInst;
        if (sf2->isSet(idInst, champ_sampleModes))
            _boucleGlobale = sf2->get(idInst, champ_sampleModes).wValue;
        else
        {
            // Toutes les divisions ont-elles le même paramètre de bouclage ?
            idInst.typeElement = elementInstSmpl;
            int nbInstSmpl = sf2->count(idInst);
            _boucleGlobale = -2;
            for (int i = 0; i < nbInstSmpl; i++)
            {
                idInst.indexElt2 = i;
                if (!sf2->get(idInst, champ_hidden).bValue)
                {
                    int valTmp = 0;
                    if (sf2->isSet(idInst, champ_sampleModes))
                        valTmp = sf2->get(idInst, champ_sampleModes).wValue;
                    if (_boucleGlobale == -2)
                        _boucleGlobale = valTmp;
                    else if (_boucleGlobale != valTmp)
                        _boucleGlobale = -1;
                }
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
                _listeValeurs << getValue(champ_initialAttenuation, sf2->get(instGlob, champ_initialAttenuation).genValue, false);
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
            _listeValeurs << getDefaultValue(champ_attackVolEnv);
        }

        // Fréquences par défaut si non définies
        if (!_listeChamps.contains(champ_freqModLFO))
        {
            _listeChamps << champ_freqModLFO;
            _listeValeurs << getDefaultValue(champ_freqModLFO);
        }
        if (!_listeChamps.contains(champ_freqVibLFO))
        {
            _listeChamps << champ_freqVibLFO;
            _listeValeurs << getDefaultValue(champ_freqVibLFO);
        }

        // Fréquence de coupure par défaut si non définie
        if (!_listeChamps.contains(champ_initialFilterFc))
        {
            _listeChamps << champ_initialFilterFc;
            _listeValeurs << getDefaultValue(champ_initialFilterFc);
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

void ParamListe::adaptKeynum2()
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
void ParamListe::adaptKeynum2(int minKey, int maxKey, Champ champBase, Champ champKeynum)
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

void ParamListe::adaptLfo(Pile_sf2 * sf2, EltID idInstSmpl)
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
            _listeValeurs << getValue(champ_delayModLFO, sf2->get(idInst, champ_delayModLFO).genValue, false);
        }
        if (!_listeChamps.contains(champ_freqModLFO) && sf2->isSet(idInst, champ_freqModLFO))
        {
            _listeChamps << champ_freqModLFO;
            _listeValeurs << getValue(champ_freqModLFO, sf2->get(idInst, champ_freqModLFO).genValue, false);
        }
        if (!_listeChamps.contains(champ_modLfoToPitch) && sf2->isSet(idInst, champ_modLfoToPitch))
        {
            _listeChamps << champ_modLfoToPitch;
            _listeValeurs << getValue(champ_modLfoToPitch, sf2->get(idInst, champ_modLfoToPitch).genValue, false);
        }
        if (!_listeChamps.contains(champ_modLfoToFilterFc) && sf2->isSet(idInst, champ_modLfoToFilterFc))
        {
            _listeChamps << champ_modLfoToFilterFc;
            _listeValeurs << getValue(champ_modLfoToFilterFc, sf2->get(idInst, champ_modLfoToFilterFc).genValue, false);
        }
        if (!_listeChamps.contains(champ_modLfoToVolume) && sf2->isSet(idInst, champ_modLfoToVolume))
        {
            _listeChamps << champ_modLfoToVolume;
            _listeValeurs << getValue(champ_modLfoToVolume, sf2->get(idInst, champ_modLfoToVolume).genValue, false);
        }
    }
    if (_listeChamps.contains(champ_vibLfoToPitch) || _listeChamps.contains(champ_delayVibLFO) ||
            _listeChamps.contains(champ_freqVibLFO))
    {
        if (!_listeChamps.contains(champ_delayVibLFO) && sf2->isSet(idInst, champ_delayVibLFO))
        {
            _listeChamps << champ_delayVibLFO;
            _listeValeurs << getValue(champ_delayVibLFO, sf2->get(idInst, champ_delayVibLFO).genValue, false);
        }
        if (!_listeChamps.contains(champ_freqVibLFO) && sf2->isSet(idInst, champ_freqVibLFO))
        {
            _listeChamps << champ_freqVibLFO;
            _listeValeurs << getValue(champ_freqVibLFO, sf2->get(idInst, champ_freqVibLFO).genValue, false);
        }
        if (!_listeChamps.contains(champ_vibLfoToPitch) && sf2->isSet(idInst, champ_vibLfoToPitch))
        {
            _listeChamps << champ_vibLfoToPitch;
            _listeValeurs << getValue(champ_vibLfoToPitch, sf2->get(idInst, champ_vibLfoToPitch).genValue, false);
        }
    }
}

double ParamListe::getValKeynum(double valBase, int key, double keynum)
{
    return valBase * qPow(2., (60. - (double)key) * keynum / 1200.);
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
        if (!_listeChamps.contains(sf2->get(id, champ_sfGenOper).sfGenValue))
        {
            Champ champ = sf2->get(id, champ_sfGenOper).sfGenValue;
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
                _listeChamps << champ;
                _listeValeurs << getValue(champ, sf2->get(id, champ_sfGenAmount).genValue, isPrst);
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

void ParamListe::getGlobalValue(Pile_sf2 * sf2, EltID id, Champ champ)
{
    // Chargement d'une valeur de la division globale
    if (!_listeChamps.contains(champ) && sf2->isSet(id, champ))
    {
        _listeChamps << champ;
        _listeValeurs << getValue(champ, sf2->get(id, champ).genValue, false);
    }
}

void ParamListe::mix(Champ champCoarse, Champ champFine, int addValue)
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
