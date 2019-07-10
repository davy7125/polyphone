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

#include "conversion_sfz.h"
#include "soundfontmanager.h"
#include "sampleutils.h"
#include "samplewriterwav.h"
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDate>
#include "contextmanager.h"
#include "attribute.h"
#include "sfzparamlist.h"


ConversionSfz::ConversionSfz() : QObject(),
    _sf2(SoundfontManager::getInstance())
{}

QString ConversionSfz::convert(QString dirPath, EltID idSf2, bool presetPrefix, bool bankDir, bool gmSort)
{
    // Create the base directory
    if (QDir(dirPath).exists())
    {
        int i = 1;
        while (QDir(dirPath + "-" + QString::number(i)).exists())
            i++;
        dirPath += "-" + QString::number(i);
    }
    if (!QDir().mkdir(dirPath))
        return trUtf8("Cannot create directory \"%1\"").arg(dirPath);

    // Plusieurs banques sont utilisées ?
    int numBankUnique = -1;
    _bankSortEnabled = false;
    EltID presetId(elementPrst, idSf2.indexSf2);
    foreach (int presetNumber, _sf2->getSiblings(presetId))
    {
        presetId.indexElt = presetNumber;
        if (numBankUnique == -1)
            numBankUnique = _sf2->get(presetId, champ_wBank).wValue;
        else
            _bankSortEnabled |= (numBankUnique != _sf2->get(presetId, champ_wBank).wValue);
    }
    if (_bankSortEnabled)
        numBankUnique = -1; // Si numBankUnique est différent de -1, il nous donne le numéro unique de banque utilisée
    _bankSortEnabled &= bankDir;
    _gmSortEnabled = gmSort;

    // Sample directory
    _dirSamples = dirPath + "/samples";
    QDir().mkdir(_dirSamples);

    // For each preset in the soundfont
    foreach (int presetNumber, _sf2->getSiblings(presetId))
    {
        presetId.indexElt = presetNumber;

        // Répertoire allant contenir le fichier sfz
        QString sourceDir = dirPath;

        int numBank = _sf2->get(presetId, champ_wBank).wValue;
        int numPreset = _sf2->get(presetId, champ_wPreset).wValue;

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

        exportPrst(sourceDir, presetId, presetPrefix);
    }

    return "";
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

        foreach (int i, _sf2->getSiblings(id))
        {
            id.indexElt2 = i;
            QList<EltID> listProcessedInstSmpl;

            // Paramètres du prst
            SfzParamList * paramPrst = new SfzParamList(_sf2, id);

            // ID de l'instrument lié
            EltID idInst = id;
            idInst.typeElement = elementInst;
            idInst.indexElt = _sf2->get(id, champ_instrument).wValue;

            // Paramètres globaux (groupe)
            SfzParamList * paramGroupe = new SfzParamList(_sf2, paramPrst, idInst);
            writeGroup(&fichierSfz, paramGroupe, numBank == 128);
            delete paramGroupe;

            // Ecriture de chaque élément présent dans l'instrument
            idInst.typeElement = elementInstSmpl;

            // Ordre des instSmpl
            QMultiMap<int, EltID> map;
            foreach (int j, _sf2->getSiblings(idInst))
            {
                idInst.indexElt2 = j;
                map.insert(_sf2->get(idInst, champ_keyRange).rValue.byLo, idInst);
            }

            QList<EltID> listInstSmpl = map.values();
            for (int j = 0; j < listInstSmpl.count(); j++)
            {
                idInst = listInstSmpl.at(j);
                if (!listProcessedInstSmpl.contains(idInst) && isIncluded(paramPrst, idInst))
                {
                    SfzParamList * paramInstSmpl = new SfzParamList(_sf2, paramPrst, idInst);
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
                                _sf2->get(idSmpl, champ_chPitchCorrection).cValue == _sf2->get(idSmplLinked, champ_chPitchCorrection).cValue &&
                                _sf2->get(idSmpl, champ_byOriginalPitch).bValue == _sf2->get(idSmplLinked, champ_byOriginalPitch).bValue &&
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
                                    QMap<AttributeType, AttributeValue> map, mapLinked;
                                    foreach (int champ, _sf2->getSiblings(idInstSmplGen))
                                    {
                                        map.insert((AttributeType)champ, _sf2->get(idInstSmplGen.parent(), (AttributeType)champ));
                                    }
                                    foreach (int champ, _sf2->getSiblings(idLinkedInstSmplGen))
                                    {
                                        mapLinked.insert((AttributeType)champ, _sf2->get(idLinkedInstSmplGen.parent(), (AttributeType)champ));
                                    }
                                    QList<AttributeType> listeChamps = map.keys();
                                    QList<AttributeType> listeChampsLinked = mapLinked.keys();
                                    QList<AttributeValue> listeValeurs = map.values();
                                    QList<AttributeValue> listeValeursLinked = mapLinked.values();
                                    if (listeChamps.size() == listeChampsLinked.size())
                                    {
                                        for (int l = 0; l < listeChamps.size(); l++)
                                        {
                                            if (listeChamps.at(l) == champ_pan)
                                                isEqual &= listeChamps.at(l) == listeChampsLinked.at(l) &&
                                                        listeValeurs.at(l).shValue == -listeValeursLinked.at(l).shValue;
                                            else if (listeChamps.at(l) != champ_sampleID)
                                                isEqual &= listeChamps.at(l) == listeChampsLinked.at(l) &&
                                                        listeValeurs.at(l).shValue == listeValeursLinked.at(l).shValue;
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
        fichierSfz.close();
    }
}

QString ConversionSfz::getPathSfz(QString dir, QString name)
{
    if (name.isEmpty())
        name = trUtf8("untitled");
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
    // Write header
    id.typeElement = elementSf2;
    QTextStream out(fichierSfz);
    out << "// Sfz exported from a sf2 file with Polyphone" << endl
        << "// Name      : " << _sf2->getQstr(id, champ_name).replace(QRegExp("[\r\n]"), " ") << endl
        << "// Author    : " << _sf2->getQstr(id, champ_IENG).replace(QRegExp("[\r\n]"), " ") << endl
        << "// Copyright : " << _sf2->getQstr(id, champ_ICOP).replace(QRegExp("[\r\n]"), " ") << endl
        << "// Date      : " << QDate::currentDate().toString("yyyy/MM/dd") << endl
        << "// Comment   : " << _sf2->getQstr(id, champ_ICMT).replace(QRegExp("[\r\n]"), " ") << endl;
}

void ConversionSfz::writeGroup(QFile * fichierSfz, SfzParamList * listeParam, bool isPercKit)
{
    // Ecriture de paramètres communs à plusieurs régions
    QTextStream out(fichierSfz);
    out << endl << "<group>" << endl;
    if (isPercKit)
        out << "lochan=10 hichan=10" << endl;
    for (int i = 0; i < listeParam->size(); i++)
        writeElement(out, listeParam->getChamp(i), listeParam->getValeur(i));
}

void ConversionSfz::writeRegion(QFile * fichierSfz, SfzParamList * listeParam, QString pathSample, bool ignorePan)
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

void ConversionSfz::writeElement(QTextStream &out, AttributeType champ, double value)
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
            << ContextManager::keyName()->getKeyName(qRound(value), false, false, true) << endl
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
            << ContextManager::keyName()->getKeyName(qRound(value), false, false, true) << endl;
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
        QString lokey = ContextManager::keyName()->getKeyName(qRound(value / 1000.), false, false, true);
        QString hikey = ContextManager::keyName()->getKeyName(qRound(value - 1000. * qRound(value / 1000.)), false, false, true);
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
            int nb = SampleUtils::lastLettersToRemove(nom1, nom2);
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
        SampleWriterWav writer(_dirSamples + "/" + name + ".wav");
        if (idSmpl.indexElt == -1 || idSmpl2.indexElt == -1)
        {
            // Mono
            if (idSmpl.indexElt2 != -1)
            {
                _mapMonoSamples.insert(idSmpl.indexElt, path);
                writer.write(_sf2->getSound(idSmpl));
            }
            else if (idSmpl2.indexElt != -1)
            {
                _mapMonoSamples.insert(idSmpl2.indexElt, path);
                writer.write(_sf2->getSound(idSmpl2));
            }
        }
        else
        {
            // Stéréo
            _mapStereoSamples.insert(idSmpl.indexElt, path);
            _mapStereoSamples.insert(idSmpl2.indexElt, path);
            writer.write(_sf2->getSound(idSmpl2), _sf2->getSound(idSmpl));
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
    QString strRet = trUtf8("other");

    if (numPreset < 8)
        strRet = "000-007 " + trUtf8("Piano");
    else if (numPreset < 16)
        strRet = "008-015 " + trUtf8("Chromatic percussion");
    else if (numPreset < 24)
        strRet = "016-023 " + trUtf8("Organ");
    else if (numPreset < 32)
        strRet = "024-031 " + trUtf8("Guitar");
    else if (numPreset < 40)
        strRet = "032-039 " + trUtf8("Bass");
    else if (numPreset < 48)
        strRet = "040-047 " + trUtf8("Strings");
    else if (numPreset < 56)
        strRet = "048-055 " + trUtf8("Ensemble");
    else if (numPreset < 64)
        strRet = "056-063 " + trUtf8("Brass");
    else if (numPreset < 72)
        strRet = "064-071 " + trUtf8("Reed");
    else if (numPreset < 80)
        strRet = "072-079 " + trUtf8("Pipe");
    else if (numPreset < 88)
        strRet = "080-087 " + trUtf8("Synth lead");
    else if (numPreset < 96)
        strRet = "088-095 " + trUtf8("Synth pad");
    else if (numPreset < 104)
        strRet = "096-103 " + trUtf8("Synth effects");
    else if (numPreset < 112)
        strRet = "104-111 " + trUtf8("Ethnic");
    else if (numPreset < 120)
        strRet = "112-119 " + trUtf8("Percussive");
    else if (numPreset < 128)
        strRet = "120-127 " + trUtf8("Sound effects");
    else if (numPreset == 128)
        strRet = trUtf8("Percussion kit");

    return strRet;
}

QString ConversionSfz::getDrumCategory(int numPreset)
{
    QString strRet = trUtf8("other");

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

bool ConversionSfz::isIncluded(SfzParamList * paramPrst, EltID idInstSmpl)
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
        RangesType range = _sf2->get(idInstSmpl, champ_keyRange).rValue;
        instMinKey = range.byLo;
        instMaxKey = range.byHi;
    }
    if (_sf2->isSet(idInstSmpl, champ_velRange))
    {
        RangesType range = _sf2->get(idInstSmpl, champ_velRange).rValue;
        instMinVel = range.byLo;
        instMaxVel = range.byHi;
    }

    return instMinKey <= prstMaxKey && instMaxKey >= prstMinKey &&
            instMinVel <= prstMaxVel && instMaxVel >= prstMinVel;
}
