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

void ConversionSfz::convert(QString dir, QList<EltID> listID, bool presetPrefix, bool bankDir, bool gmSort)
{
    if (listID.isEmpty())
        return;

    // Création d'un répertoire
    EltID idSF2 = listID.at(0);
    idSF2.typeElement = elementSf2;
    QString rootDir = QFileInfo(_sf2->getQstr(idSF2, champ_filename)).baseName();
    QDir dossier(dir + "/" + rootDir);
    if (dossier.exists())
    {
        int i = 1;
        while (QDir(dir + "/" + rootDir + "-" + QString::number(i)).exists())
            i++;
        rootDir += "-" + QString::number(i);
    }
    rootDir = dir + "/" + rootDir;
    QDir(rootDir).mkdir(rootDir);

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

    return;
}

void ConversionSfz::exportPrst(QString dir, EltID id, bool presetPrefix)
{
    QString numText;
    if (presetPrefix)
        numText.sprintf("%.3u_", _sf2->get(id, champ_wPreset).wValue);
    int numBank = _sf2->get(id, champ_wBank).wValue;
    QString path = getPathSfz(dir, numText + _sf2->getQstr(id, champ_name)) + ".sfz";
    QFile fichierSfz(path);
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
                writeGroup(&fichierSfz, paramGroupe, numBank == 128);
                delete paramGroupe;

                // Ecriture de chaque élément présent dans l'instrument
                idInst.typeElement = elementInstSmpl;
                for (int j = 0; j < _sf2->count(idInst); j++)
                {
                    idInst.indexElt2 = j;
                    if (!_sf2->get(idInst, champ_hidden).bValue)
                    {
                        ParamListe * paramInstSmpl = new ParamListe(_sf2, paramPrst, idInst);
                        EltID idSmpl = idInst;
                        idSmpl.typeElement = elementSmpl;
                        idSmpl.indexElt = _sf2->get(idInst, champ_sampleID).wValue;
                        writeRegion(&fichierSfz, paramInstSmpl, getLink(idSmpl));
                        delete paramInstSmpl;
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
        << QObject::trUtf8("// Commentaire : ") << _sf2->getQstr(id, champ_ICMT).replace(QRegExp("[\r\n]"), " ") << endl
        << endl;
}

void ConversionSfz::writeGroup(QFile * fichierSfz, ParamListe * listeParam, bool isPercKit)
{
    // Ecriture de paramètres communs à plusieurs régions
    QTextStream out(fichierSfz);
    out << "<group>" << endl;
    if (isPercKit)
        out << "lochan=10 hichan=10" << endl;
    for (int i = 0; i < listeParam->size(); i++)
        writeElement(out, listeParam->getChamp(i), listeParam->getValeur(i));
    out << endl;
}

void ConversionSfz::writeRegion(QFile * fichierSfz, ParamListe * listeParam, QString pathSample)
{
    // Ecriture de paramètres spécifique à une région
    QTextStream out(fichierSfz);
    out << "<region>" << endl
        << "sample=" << pathSample.replace("/", "\\") << endl;
    for (int i = 0; i < listeParam->size(); i++)
        writeElement(out, listeParam->getChamp(i), listeParam->getValeur(i));
    out << endl;
}

void ConversionSfz::writeElement(QTextStream &out, Champ champ, double value)
{
    QString v2 = " // sfz v2";
    switch (champ)
    {
    case champ_fineTune:                out << "tune=" << (int)(value + 0.5) << endl;               break;
    case champ_coarseTune:              out << "transpose=" << (int)(value + 0.5) << endl;          break;
    case champ_scaleTuning:             out << "pitch_keytrack=" << (int)(value + 0.5) << endl;     break;
    case champ_startloopAddrsOffset:    out << "loop_start=" << (int)(value + 0.5) << endl;         break;
    case champ_startAddrsOffset:        out << "offset=" << (int)(value + 0.5) << endl;             break;
    case champ_endloopAddrsOffset:      out << "loop_end=" << (int)(value + 0.5) - 1 << endl;       break;
    case champ_endAddrsOffset:          out << "end=" << (int)(value + 0.5) << endl;                break;
    case champ_pan:                     out << "pan=" << 2 * value << endl;                         break;
    case champ_initialAttenuation:      out << "volume=" << -value * 0.68 << endl;                  break;
    case champ_initialFilterQ:          out << "resonance=" << value << endl;                       break;
    case champ_sustainModEnv:           out << "fileg_sustain=" << dbToPercent(value) << endl
                                            << "pitcheg_sustain=" << dbToPercent(value) << endl;    break;
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
    case champ_modEnvToPitch:           out << "pitcheg_depth=" << (int)(value + 0.5) << endl;      break;
    case champ_modEnvToFilterFc:        out << "fileg_depth=" << (int)(value + 0.5) << endl;        break;
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
    case champ_keynum:                  out << "pitch_keycenter=" << (int)(value + 0.5) << endl
                                            << "pitch_keytrack=0" << endl;                          break;
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
    case champ_overridingRootKey:       out << "pitch_keycenter=" << (int)(value + 0.5) << endl;    break;
    case champ_delayVibLFO:             out << "pitchlfo_delay=" << value << endl;                  break;
    case champ_freqVibLFO:              out << "pitchlfo_freq=" << value << endl;                   break;
    case champ_vibLfoToPitch:           out << "pitchlfo_depth=" << (int)(value + 0.5) << endl;     break;
    case champ_velocity:                out << "amp_velcurve_1=" << value / 127. << endl
                                            << "amp_velcurve_127=" << value / 127. << endl;         break;
    case champ_exclusiveClass:
        if (value != 0)
            out << "group=" << (int)(value + 0.5) << endl
                << "off_by=" << (int)(value + 0.5) << endl;
        break;
    case champ_initialFilterFc:
        out << "fil_type=lpf_2p" << endl
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

QString ConversionSfz::getLink(EltID idSmpl)
{
    QString path = "";
    if (_sampleIDs.contains(idSmpl.indexElt))
        path = _samplePaths.at(_sampleIDs.indexOf(idSmpl.indexElt));
    else
    {
        QString name;
        EltID idSmpl2 = idSmpl;
        idSmpl2.indexElt = -1;

        // Stéréo ?
        if (_sf2->get(idSmpl, champ_sfSampleType).wValue != monoSample &&
                _sf2->get(idSmpl, champ_sfSampleType).wValue != RomMonoSample)
        {
            idSmpl2.indexElt = _sf2->get(idSmpl, champ_wSampleLink).wValue;

            // Nom du fichier
            QString nom1 = _sf2->getQstr(idSmpl, champ_name);
            QString nom2 = _sf2->getQstr(idSmpl2, champ_name);
            int nb = Sound::lastLettersToRemove(nom1, nom2);
            name = nom1.left(nom1.size() - nb);

            if (_sf2->get(idSmpl, champ_sfSampleType).wValue == rightSample &&
                    _sf2->get(idSmpl, champ_sfSampleType).wValue != RomRightSample)
            {
                // Inversion smpl1 smpl2
                EltID idTmp = idSmpl;
                idSmpl = idSmpl2;
                idSmpl2 = idTmp;
            }
        }
        else
        {
            name = _sf2->getQstr(idSmpl, champ_name);
        }

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
            if (idSmpl.indexElt2 != -1)
            {
                _sampleIDs << idSmpl.indexElt;
                _samplePaths << path;
                Sound::exporter(_dirSamples + "/" + name + ".wav", _sf2->getSon(idSmpl));
            }
            else if (idSmpl2.indexElt != -1)
            {
                _sampleIDs << idSmpl2.indexElt;
                _samplePaths << path;
                Sound::exporter(_dirSamples + "/" + name + ".wav", _sf2->getSon(idSmpl2));
            }
        }
        else
        {
            _sampleIDs << idSmpl.indexElt << idSmpl2.indexElt;
            _samplePaths << path << path;
            Sound::exporter(_dirSamples + "/" + name + ".wav", _sf2->getSon(idSmpl), _sf2->getSon(idSmpl2));
        }
    }
    return path;
}

QString ConversionSfz::escapeStr(QString str)
{
    //return str.replace(QRegExp(QString::fromUtf8("[`~!@$%^*|:;<>«»,.?/{}\'\"\\\[\\]\\\\]")), "_");
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

        // Pas de boucle si non défini
        if (!_listeChamps.contains(champ_sampleModes))
        {
            _listeChamps << champ_sampleModes;
            _listeValeurs << 0;
        }
        idInst.typeElement = elementInst;
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
            _listeChamps << champ_overridingRootKey;
            _listeValeurs << sf2->get(idSmpl, champ_byOriginalPitch).bValue;
        }

        if (!_listeChamps.contains(champ_fineTune))
        {
            _listeChamps << champ_fineTune;
            _listeValeurs << sf2->get(idSmpl, champ_chPitchCorrection).cValue;
        }
        else
        {
            _listeValeurs[_listeChamps.indexOf(champ_fineTune)] +=
                    sf2->get(idSmpl, champ_chPitchCorrection).cValue;
        }

        // Adaptation des offsets
        mix(champ_startAddrsCoarseOffset, champ_startAddrsOffset);
        mix(champ_endAddrsCoarseOffset, champ_endAddrsOffset, sf2->get(idSmpl, champ_dwLength).dwValue);
        mix(champ_startloopAddrsCoarseOffset, champ_startloopAddrsOffset, sf2->get(idSmpl, champ_dwStartLoop).dwValue);
        mix(champ_endloopAddrsCoarseOffset, champ_endloopAddrsOffset, sf2->get(idSmpl, champ_dwEndLoop).dwValue);

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
        {
            if (_listeChamps.contains(paramPrst->_listeChamps.at(i)))
                fusion(paramPrst->_listeChamps.at(i), paramPrst->_listeValeurs.at(i));
        }
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

    // Fréquence par défaut si non défini
    if ((_listeChamps.contains(champ_modLfoToFilterFc) || _listeChamps.contains(champ_modLfoToPitch) ||
         _listeChamps.contains(champ_modLfoToVolume)) && !_listeChamps.contains(champ_freqModLFO))
    {
        _listeChamps << champ_freqModLFO;
        _listeValeurs << getDefaultValue(champ_freqModLFO);
    }
    if (_listeChamps.contains(champ_vibLfoToPitch) && !_listeChamps.contains(champ_freqVibLFO))
    {
        _listeChamps << champ_freqVibLFO;
        _listeValeurs << getDefaultValue(champ_freqVibLFO);
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
                        champ != champ_keynum))
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
