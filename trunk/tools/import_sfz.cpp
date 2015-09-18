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

#include <QTextStream>
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include "import_sfz.h"
#include "pile_sf2.h"
#include "config.h"


ImportSfz::ImportSfz(Pile_sf2 * sf2) :
    _sf2(sf2),
    _currentBloc(BLOC_UNKNOWN)
{
}

void ImportSfz::import(QString fileName, int * numSf2)
{
    QFile inputFile(fileName);
    if (inputFile.open(QIODevice::ReadOnly))
    {
        QTextStream in(&inputFile);
        while (!in.atEnd())
        {
            // Lecture ligne par ligne
            QString line = in.readLine();

            // Suppression des commentaires
            if (line.contains("//"))
                line = line.left(line.indexOf("//"));

            // Découpage
            QStringList list = line.split(" ", QString::KeepEmptyParts);
            int length = list.size();
            for (int i = length - 1; i >= 1; i--)
            {
                if (!list.at(i).contains(QRegExp("[<>=]")))
                {
                    list[i-1] += " " + list[i];
                    list.removeAt(i);
                }
            }

            // Suppression des espaces de fin et des champs vides
            length = list.size();
            for (int i = length - 1; i >= 0; i--)
            {
                QString strTmp = list.at(i);
                while (strTmp.endsWith(' '))
                    strTmp.chop(1);

                if (strTmp.isEmpty())
                    list.removeAt(i);
                else
                    list[i] = strTmp;
            }

            // Stockage
            for (int i = 0; i < list.size(); i++)
            {
                QString str = list.at(i);

                // Valide ?
                if (str.size() > 2)
                {
                    if (str.left(1) == "<" && str.right(1) == ">")
                        changeBloc(str.right(str.size()-1).left(str.size()-2));
                    else if (str.contains("="))
                    {
                        int index = str.indexOf("=");
                        QString opcode = str.left(index).toLower();
                        QString value = str.right(str.length() - index - 1);
                        if (opcode.size() && value.size())
                            addOpcode(opcode, value);
                    }
                }
            }
        }
        inputFile.close();

        // Les samples doivent être valides
        // Les offsets doivent se trouver à côté des samples, pas dans les divisions globales
        // Idem pan, samples, off_by et group
        // On ne garde que les filtres supportés par le format sf2
        // Ajustement du volume des sons stéréo
        // Ajustement du volume si le modulation de volume est appliqué
        // Recherche du canal 10
        // Recherche de l'amplification max
        bool isChannel10 = true;
        double ampliMax = 0;
        for (int i = 0; i < _listeEnsembles.size(); i++)
        {
            _listeEnsembles[i].moveOpcodeInSamples(Parametre::op_sample, QVariant::String);
            _listeEnsembles[i].checkSampleValid(QFileInfo(fileName).path());
            _listeEnsembles[i].moveOpcodeInSamples(Parametre::op_offset, QVariant::Int);
            _listeEnsembles[i].moveOpcodeInSamples(Parametre::op_end, QVariant::Int);
            _listeEnsembles[i].moveOpcodeInSamples(Parametre::op_loop_start, QVariant::Int);
            _listeEnsembles[i].moveOpcodeInSamples(Parametre::op_loop_end, QVariant::Int);
            _listeEnsembles[i].moveOpcodeInSamples(Parametre::op_loop_mode, QVariant::String);
            _listeEnsembles[i].moveOpcodeInSamples(Parametre::op_pan, QVariant::Double);
            _listeEnsembles[i].moveOpcodeInSamples(Parametre::op_off_by, QVariant::Int);
            _listeEnsembles[i].moveOpcodeInSamples(Parametre::op_exclusiveClass, QVariant::Int);
            _listeEnsembles[i].moveModInSamples();
            _listeEnsembles[i].moveKeynumInSamples(Parametre::op_noteToVolEnvDecay, Parametre::op_ampeg_decay);
            _listeEnsembles[i].moveKeynumInSamples(Parametre::op_noteToVolEnvHold, Parametre::op_ampeg_hold);
            _listeEnsembles[i].moveKeynumInSamples(Parametre::op_noteToModEnvDecay, Parametre::op_pitcheg_decay);
            _listeEnsembles[i].moveKeynumInSamples(Parametre::op_noteToModEnvHold, Parametre::op_pitcheg_hold);
            _listeEnsembles[i].moveKeynumInSamples(Parametre::op_fileg_decaycc133, Parametre::op_fileg_decay);
            _listeEnsembles[i].moveKeynumInSamples(Parametre::op_fileg_holdcc133, Parametre::op_fileg_hold);
            _listeEnsembles[i].adjustStereoVolumeAndCorrection(QFileInfo(fileName).path());
            _listeEnsembles[i].adjustModulationVolume();
            _listeEnsembles[i].checkFilter();
            ampliMax = qMax(ampliMax, _listeEnsembles[i].getAmpliMax());
            isChannel10 &= _listeEnsembles[i].isChannel10();
        }

        // Ajustement du volume si amplification max > 0
        if (ampliMax > 0)
            for (int i = 0; i < _listeEnsembles.size(); i++)
                _listeEnsembles[i].adjustVolume(ampliMax);

        // Simplification des atténuations
        for (int i = 0; i < _listeEnsembles.size(); i++)
            _listeEnsembles[i].simplifyAttenuation();

        // Création d'un sf2
        int numBank = 0;
        int numPreset = 0;
        QString nom = getNomInstrument(fileName, numBank, numPreset);
        if (isChannel10)
            numBank = 128;
        EltID idSf2(elementSf2, -1, -1, -1, -1);
        if (*numSf2 == -1)
        {
            idSf2.indexSf2 = _sf2->add(idSf2, false);
            *numSf2 = idSf2.indexSf2;
            _sf2->set(idSf2, champ_name, nom, false);
        }
        else
        {
            idSf2.indexSf2 = *numSf2;
            _sf2->set(idSf2, champ_name, QObject::trUtf8("Import sfz"), false);
        }
        _sf2->set(idSf2, champ_ICMT, QObject::trUtf8("Sf2 importé depuis un fichier sfz par Polyphone"), false);
        _sf2->closestAvailablePreset(idSf2, numBank, numPreset);

        // Création d'un preset
        EltID idPrst = idSf2;
        idPrst.typeElement = elementPrst;
        idPrst.indexElt = _sf2->add(idPrst, false);
        _sf2->set(idPrst, champ_name, nom, false);
        Valeur val;
        val.wValue = numBank;
        _sf2->set(idPrst, champ_wBank, val, false);
        val.wValue = numPreset;
        _sf2->set(idPrst, champ_wPreset, val, false);

        // Création des instruments
        EltID idInst = idSf2;
        idInst.typeElement = elementInst;
        for (int i = 0; i < _listeEnsembles.size(); i++)
        {
            idInst.indexElt = _sf2->add(idInst, false);
            QString nomInst = nom;
            if (_listeEnsembles.size() > 9)
                nomInst = nom.left(17) + "-" + QString::number(i+1);
            else if (_listeEnsembles.size() > 1)
                nomInst = nom.left(18) + "-" + QString::number(i+1);
            else
                nomInst = nom;
            _sf2->set(idInst, champ_name, nomInst, false);

            EltID idInstSmpl = idInst;
            idInstSmpl.typeElement = elementInstSmpl;

            // Lien dans le preset
            EltID idPrstInst = idPrst;
            idPrstInst.typeElement = elementPrstInst;
            idPrstInst.indexElt2 = _sf2->add(idPrstInst, false);
            val.dwValue = idInst.indexElt;
            _sf2->set(idPrstInst, champ_instrument, val, false);

            // Remplissage de l'instrument et création des samples
            _listeEnsembles[i].decode(_sf2, idInst, QFileInfo(fileName).path());

            // Détermination keyRange et velRange du preset
            int nbInstSmpl = _sf2->count(idInstSmpl);
            int keyMin = 127;
            int keyMax = 0;
            int velMin = 127;
            int velMax = 0;
            for (int j = 0; j < nbInstSmpl; j++)
            {
                idInstSmpl.indexElt2 = j;
                if (_sf2->isSet(idInstSmpl, champ_keyRange))
                {
                    rangesType range = _sf2->get(idInstSmpl, champ_keyRange).rValue;
                    keyMin = qMin(keyMin, (int)range.byLo);
                    keyMax = qMax(keyMax, (int)range.byHi);
                }
                if (_sf2->isSet(idInstSmpl, champ_velRange))
                {
                    rangesType range = _sf2->get(idInstSmpl, champ_velRange).rValue;
                    velMin = qMin(velMin, (int)range.byLo);
                    velMax = qMax(velMax, (int)range.byHi);
                }
            }
            if (_sf2->isSet(idInst, champ_keyRange))
            {
                rangesType range = _sf2->get(idInst, champ_keyRange).rValue;
                keyMin = qMin(keyMin, (int)range.byLo);
                keyMax = qMax(keyMax, (int)range.byHi);
            }
            if (_sf2->isSet(idInst, champ_velRange))
            {
                rangesType range = _sf2->get(idInst, champ_velRange).rValue;
                velMin = qMin(velMin, (int)range.byLo);
                velMax = qMax(velMax, (int)range.byHi);
            }
            if (keyMin > keyMax)
            {
                keyMin = 0;
                keyMax = 127;
            }
            val.rValue.byLo = keyMin;
            val.rValue.byHi = keyMax;
            _sf2->set(idPrstInst, champ_keyRange, val, false);
            if (velMin <= velMax)
            {
                val.rValue.byLo = velMin;
                val.rValue.byHi = velMax;
                _sf2->set(idPrstInst, champ_velRange, val, false);
            }

            // Suppression des atténuations, corrections, type loop et exclusive class inutiles
            if (!_sf2->isSet(idInst, champ_initialAttenuation))
            {
                for (int i = 0; i < _sf2->count(idInstSmpl); i++)
                {
                    idInstSmpl.indexElt2 = i;
                    if (_sf2->get(idInstSmpl, champ_initialAttenuation).wValue == 0)
                        _sf2->reset(idInstSmpl, champ_initialAttenuation, false);
                }
            }
            if (!_sf2->isSet(idInst, champ_fineTune))
            {
                for (int i = 0; i < _sf2->count(idInstSmpl); i++)
                {
                    idInstSmpl.indexElt2 = i;
                    if (_sf2->get(idInstSmpl, champ_fineTune).wValue == 0)
                        _sf2->reset(idInstSmpl, champ_fineTune, false);
                }
            }
            int typeLoop = _sf2->get(idInst, champ_sampleModes).wValue;
            if (typeLoop == 0)
                _sf2->reset(idInst, champ_sampleModes);
            for (int i = 0; i < _sf2->count(idInstSmpl); i++)
            {
                idInstSmpl.indexElt2 = i;
                if (_sf2->get(idInstSmpl, champ_sampleModes).wValue == typeLoop)
                    _sf2->reset(idInstSmpl, champ_sampleModes, false);
            }
            if (!_sf2->isSet(idInst, champ_exclusiveClass))
            {
                for (int i = 0; i < _sf2->count(idInstSmpl); i++)
                {
                    idInstSmpl.indexElt2 = i;
                    if (_sf2->get(idInstSmpl, champ_exclusiveClass).wValue == 0)
                        _sf2->reset(idInstSmpl, champ_exclusiveClass, false);
                }
            }

            // Suppression keyrange et velocity range de la division globale de l'instrument
            _sf2->reset(idInst, champ_velRange, false);
            _sf2->reset(idInst, champ_keyRange, false);
        }
    }
}

QString ImportSfz::getNomInstrument(QString filePath, int &numBank, int &numPreset)
{
    QFileInfo fileInfo(filePath);
    QString nomFichier = fileInfo.completeBaseName();
    QString nomDir = fileInfo.dir().dirName();

    // Numéro de preset
    QRegExp regExp("^\\d\\d\\d.*");
    if (regExp.exactMatch(nomFichier))
    {
        numPreset = nomFichier.left(3).toInt();
        if (numPreset < 0 || numPreset > 127)
            numPreset = 0;
        nomFichier = nomFichier.right(nomFichier.length() - 3);
        if (!nomFichier.isEmpty())
        {
            QString ch = nomFichier.left(1);
            if (ch.compare("-") == 0 || ch.compare("_") == 0 || ch.compare(".") == 0 || ch.compare(" ") == 0)
                nomFichier = nomFichier.right(nomFichier.length() - 1);
        }
    }

    // Numéro de banque
    if (regExp.exactMatch(nomDir))
    {
        numBank = nomDir.left(3).toInt();
        if (numBank < 0 || numBank > 127)
            numBank = 0;
    }

    // Nom de l'instrument
    if (nomFichier.isEmpty())
        nomFichier = QObject::trUtf8("sans nom");
    return nomFichier.left(20);
}

void ImportSfz::changeBloc(QString bloc)
{
    if (bloc == "group")
    {
        _currentBloc = BLOC_GROUP;
        _listeEnsembles << EnsembleGroupes();
    }
    else if (bloc == "region")
    {
        _currentBloc = BLOC_REGION;
        if (_listeEnsembles.isEmpty())
            _listeEnsembles << EnsembleGroupes();
        _listeEnsembles.last().newGroup();
    }
    else
        _currentBloc = BLOC_UNKNOWN;
}

void ImportSfz::addOpcode(QString opcode, QString value)
{
    if (_currentBloc == BLOC_GROUP || _currentBloc == BLOC_REGION)
        _listeEnsembles.last().addParam(opcode, value);
}

void EnsembleGroupes::moveKeynumInSamples(Parametre::OpCode opCodeKeynum, Parametre::OpCode opCodeBase)
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
                _listeDivisions[i] << Parametre(opCodeKeynum, valueKeynum);
            if (!_listeDivisions.at(i).isDefined(opCodeBase))
                _listeDivisions[i] << Parametre(opCodeBase, valueBase);
        }

        _paramGlobaux.removeOpCode(opCodeKeynum);
    }
}

void EnsembleGroupes::moveOpcodeInSamples(Parametre::OpCode opcode, QVariant::Type type)
{
    switch (type)
    {
    case QVariant::Double:
        if (_paramGlobaux.isDefined(opcode))
        {
            double value = _paramGlobaux.getDoubleValue(opcode);
            for (int i = 0; i < _listeDivisions.size(); i++)
                if (!_listeDivisions.at(i).isDefined(opcode))
                    _listeDivisions[i] << Parametre(opcode, value);
        }
        break;
    case QVariant::Int:
        if (_paramGlobaux.isDefined(opcode))
        {
            int value = _paramGlobaux.getIntValue(opcode);
            for (int i = 0; i < _listeDivisions.size(); i++)
                if (!_listeDivisions.at(i).isDefined(opcode))
                    _listeDivisions[i] << Parametre(opcode, value);
        }
        break;
    case QVariant::String:
        if (_paramGlobaux.isDefined(opcode))
        {
            QString value = _paramGlobaux.getStrValue(opcode);
            for (int i = 0; i < _listeDivisions.size(); i++)
                if (!_listeDivisions.at(i).isDefined(opcode))
                    _listeDivisions[i] << Parametre(opcode, value);
        }
        break;
    default:
        break;
    }
    _paramGlobaux.removeOpCode(opcode);
}

void EnsembleGroupes::moveModInSamples()
{
    QList<Parametre::OpCode> listTmp;
    listTmp << Parametre::op_modEnvToTon
            << Parametre::op_pitcheg_attack
            << Parametre::op_pitcheg_decay
            << Parametre::op_pitcheg_delay
            << Parametre::op_pitcheg_hold
            << Parametre::op_pitcheg_release
            << Parametre::op_pitcheg_sustain;
    moveModInSamples(listTmp);

    listTmp.clear();
    listTmp << Parametre::op_modEnvToFilter
            << Parametre::op_fileg_attack
            << Parametre::op_fileg_decay
            << Parametre::op_fileg_delay
            << Parametre::op_fileg_hold
            << Parametre::op_fileg_release
            << Parametre::op_fileg_sustain;
    moveModInSamples(listTmp);

    listTmp.clear();
    listTmp << Parametre::op_modLFOtoVolume
            << Parametre::op_modLFOdelay
            << Parametre::op_modLFOfreq;
    moveModInSamples(listTmp);

    listTmp.clear();
    listTmp << Parametre::op_modLFOtoFilter
            << Parametre::op_filLFOdelay
            << Parametre::op_filLFOfreq;
    moveModInSamples(listTmp);
}

void EnsembleGroupes::moveModInSamples(QList<Parametre::OpCode> opCodeList)
{
    for (int i = 0; i < _listeDivisions.size(); i++)
    {
        for (int j = 0; j < opCodeList.size(); j++)
        {
            Parametre::OpCode opCode = opCodeList.at(j);
            if (!_listeDivisions.at(i).isDefined(opCode) && _paramGlobaux.isDefined(opCode))
            {
                // On regarde si d'autres opcodes associés sont présents
                bool isPresent = false;
                for (int k = 0; k < opCodeList.size(); k++)
                    if (k != j)
                        isPresent |= _listeDivisions.at(i).isDefined(opCodeList.at(k));
                if (isPresent)
                {
                    if (opCode == Parametre::op_modEnvToTon || opCode == Parametre::op_modEnvToFilter ||
                            opCode == Parametre::op_modLFOtoVolume || opCode == Parametre::op_modLFOtoFilter)
                        _listeDivisions[i] << Parametre(opCode, _paramGlobaux.getIntValue(opCode));
                    else
                        _listeDivisions[i] << Parametre(opCode, _paramGlobaux.getDoubleValue(opCode));
                }
            }
        }
    }
}

void EnsembleGroupes::adjustVolume(double offset)
{
    _paramGlobaux.adjustVolume(offset);
    for (int i = 0; i < _listeDivisions.size(); i++)
        _listeDivisions[i].adjustVolume(offset);
}

void EnsembleGroupes::adjustStereoVolumeAndCorrection(QString path)
{
    for (int i = 0; i < _listeDivisions.size(); i++)
        _listeDivisions[i].adjustStereoVolumeAndCorrection(path, _paramGlobaux.getIntValue(Parametre::op_tuningFine));
}

void EnsembleGroupes::adjustModulationVolume()
{
    double correctionGlobale = 0;
    if (_paramGlobaux.isDefined(Parametre::op_modLFOtoVolume))
    {
        correctionGlobale = qAbs(_paramGlobaux.getDoubleValue(Parametre::op_modLFOtoVolume));
        _paramGlobaux.adjustVolume(-correctionGlobale);
    }
    for (int i = 0; i < _listeDivisions.size(); i++)
    {
        double correction;
        if (_listeDivisions.at(i).isDefined(Parametre::op_modLFOtoVolume))
            correction = qAbs(_listeDivisions.at(i).getDoubleValue(Parametre::op_modLFOtoVolume));
        else
            correction = correctionGlobale;
        if (correction != 0)
            _listeDivisions[i].adjustVolume(-correction);
    }
}

void EnsembleGroupes::checkSampleValid(QString path)
{
    int size = _listeDivisions.size();
    for (int i = size - 1; i >= 0; i--)
        if (!_listeDivisions.at(i).sampleValid(path))
            _listeDivisions.removeAt(i);
}

void EnsembleGroupes::checkFilter()
{
    _paramGlobaux.checkFilter();
    for (int i = 0; i < _listeDivisions.size(); i++)
        _listeDivisions[i].checkFilter();
}

void EnsembleGroupes::simplifyAttenuation()
{
    bool attenuationUnique = true;
    double attenuation = -1;
    double defaultAttenuation = _paramGlobaux.getDoubleValue(Parametre::op_volume);
    for (int i = 0; i < _listeDivisions.size(); i++)
    {
        double dTmp = defaultAttenuation;
        if (_listeDivisions[i].isDefined(Parametre::op_volume))
            dTmp = _listeDivisions[i].getDoubleValue(Parametre::op_volume);
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
            _listeDivisions[i].removeOpCode(Parametre::op_volume);
        if (attenuation == 0)
            _paramGlobaux.removeOpCode(Parametre::op_volume);
        else
        {
            if (_paramGlobaux.isDefined(Parametre::op_volume))
                _paramGlobaux.adjustVolume(_paramGlobaux.getDoubleValue(Parametre::op_volume) - attenuation);
            else
                _paramGlobaux << Parametre(Parametre::op_volume, attenuation);
        }
    }
}

bool EnsembleGroupes::isChannel10()
{
    return (_paramGlobaux.getIntValue(Parametre::op_chanMin) == 10 &&
            _paramGlobaux.getIntValue(Parametre::op_chanMax) == 10);
}

double EnsembleGroupes::getAmpliMax()
{
    double ampliMax = _paramGlobaux.getDoubleValue(Parametre::op_volume);
    for (int i = 0; i < _listeDivisions.size(); i++)
        ampliMax = qMax(ampliMax, _listeDivisions.at(i).getDoubleValue(Parametre::op_volume));
    return ampliMax;
}

void EnsembleGroupes::decode(Pile_sf2 * sf2, EltID idInst, QString pathSfz)
{
    // Remplissage des paramètres globaux
    _paramGlobaux.decode(sf2, idInst);

    // Lien avec samples
    EltID idInstSmpl = idInst;
    idInstSmpl.typeElement = elementInstSmpl;
    EltID idSmpl = idInst;
    idSmpl.typeElement = elementSmpl;
    Valeur val;
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

        if (_listeDivisions.at(i).isDefined(Parametre::op_pan) && listeIndexSmpl.size() == 2)
        {
            double pan = _listeDivisions.at(i).getDoubleValue(Parametre::op_pan);
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
            idInstSmpl.indexElt2 = sf2->add(idInstSmpl, false);

            // Lien avec le sample
            val.wValue = listeIndexSmpl.first();
            sf2->set(idInstSmpl, champ_sampleID, val, false);

            // Remplissage paramètres de la division
            _listeDivisions.at(i).decode(sf2, idInstSmpl);

            if (_listeDivisions.at(i).isDefined(Parametre::op_pan))
            {
                val.shValue = 5 * _listeDivisions.at(i).getDoubleValue(Parametre::op_pan);
                sf2->set(idInstSmpl, champ_pan, val, false);
            }
        }
        else if (listeIndexSmpl.size() == 2) // Sample stereo
        {
            // Gestion width / position
            double width = 500;
            if (_listeDivisions.at(i).isDefined(Parametre::op_width))
                width = 5. * _listeDivisions.at(i).getDoubleValue(Parametre::op_width);
            double position = 0;
            if (_listeDivisions.at(i).isDefined(Parametre::op_position))
                position = _listeDivisions.at(i).getDoubleValue(Parametre::op_position) / 100.;
            if (position < 0)
                position = -qAbs(position * (500 - qAbs(width)));
            else
                position = qAbs(position * (500 - qAbs(width)));

            // Gestion pan
            double attenuation = 0;
            int panDefined = -1;
            if (_listeDivisions.at(i).isDefined(Parametre::op_pan))
            {
                double pan = _listeDivisions.at(i).getDoubleValue(Parametre::op_pan);
                attenuation = -GroupeParametres::percentToDB(100 - qAbs(pan)) * 1.7;
                if (pan < 0)
                    panDefined = 1;
                else if (pan > 0)
                    panDefined = 0;
            }

            for (int j = 0; j < listeIndexSmpl.size(); j++)
            {
                // Création InstSmpl
                idInstSmpl.indexElt2 = sf2->add(idInstSmpl, false);

                // Lien avec le sample
                val.wValue = listeIndexSmpl.at(j);
                sf2->set(idInstSmpl, champ_sampleID, val, false);

                // Pan
                if (panDefined == j)
                {
                    val.shValue = qRound(10. * attenuation);
                    sf2->set(idInstSmpl, champ_initialAttenuation, val, false);
                }

                // Remplissage paramètres de la division
                _listeDivisions.at(i).decode(sf2, idInstSmpl);

                // Width, position
                if (j == 0)
                    val.shValue = -width + position;
                else
                    val.shValue = width + position;
                sf2->set(idInstSmpl, champ_pan, val, false);
            }
        }
    }
}

QList<int> GroupeParametres::getSampleIndex(Pile_sf2 *sf2, EltID idElt, QString pathSfz) const
{
    QList<int> sampleIndex;

    int indexOpSample = -1;
    for (int i = 0; i < _listeParam.size(); i++)
        if (_listeParam.at(i).getOpCode() == Parametre::op_sample)
            indexOpSample = i;

    if (indexOpSample == -1)
        return sampleIndex;

    // Reconstitution adresse du fichier
    QString filePath =  _listeParam.at(indexOpSample).getStringValue();
    QString fileName = pathSfz + "/" + filePath.replace("\\", "/");
    if (!QFile(fileName).exists())
    {
        QStringList list = getFullPath(pathSfz, filePath.split("/", QString::SkipEmptyParts));
        if (!list.isEmpty())
            fileName = list.first();
        else
            return sampleIndex;
    }

    // Sample déjà chargé ?
    idElt.typeElement = elementSmpl;
    int nbSmpl = sf2->count(idElt);
    QStringList names;
    for (int i = 0; i < nbSmpl; i++)
    {
        idElt.indexElt = i;
        if (sf2->getQstr(idElt, champ_filename) == fileName)
            sampleIndex << i;
        names << sf2->getQstr(idElt, champ_name);
    }
    if (!sampleIndex.isEmpty())
        return sampleIndex;

    // Récupération des informations d'un sample
    Sound son(fileName, false);
    int nChannels = son.get(champ_wChannels);
    QString nom = QFileInfo(fileName).completeBaseName();
    QString nom2 = nom;

    // Adaptation du nom, éventuellement
    int suffixNumber = 0;
    if (nChannels == 2)
    {
        while ((names.contains(getName(nom, 20, suffixNumber, "L"), Qt::CaseInsensitive) ||
                names.contains(getName(nom, 20, suffixNumber, "R"), Qt::CaseInsensitive)) &&
               suffixNumber < 100)
        {
            suffixNumber++;
        }
        nom2 = getName(nom, 20, suffixNumber, "L");
        nom = getName(nom, 20, suffixNumber, "R");
    }
    else
    {
        while (names.contains(getName(nom, 20, suffixNumber), Qt::CaseInsensitive) && suffixNumber < 100)
        {
            suffixNumber++;
        }
        nom = getName(nom, 20, suffixNumber);
    }

    // Création d'un nouveau sample
    Valeur val;
    for (int numChannel = 0; numChannel < nChannels; numChannel++)
    {
        idElt.indexElt = sf2->add(idElt, false);
        sampleIndex << idElt.indexElt;
        if (nChannels == 2)
        {
            if (numChannel == 0)
            {
                // Gauche
                sf2->set(idElt, champ_name, nom2, false);
                val.wValue = idElt.indexElt + 1;
                sf2->set(idElt, champ_wSampleLink, val, false);
                val.sfLinkValue = leftSample;
                sf2->set(idElt, champ_sfSampleType, val, false);
            }
            else
            {
                // Droite
                sf2->set(idElt, champ_name, nom, false);
                val.wValue = idElt.indexElt - 1;
                sf2->set(idElt, champ_wSampleLink, val, false);
                val.sfLinkValue = rightSample;
                sf2->set(idElt, champ_sfSampleType, val, false);
            }
        }
        else
        {
            sf2->set(idElt, champ_name, nom, false);
            val.wValue = 0;
            sf2->set(idElt, champ_wSampleLink, val, false);
            val.sfLinkValue = monoSample;
            sf2->set(idElt, champ_sfSampleType, val, false);
        }
        sf2->set(idElt, champ_filename, fileName, false);
        val.dwValue = son.get(champ_dwStart16);
        sf2->set(idElt, champ_dwStart16, val, false);
        val.dwValue = son.get(champ_dwStart24);
        sf2->set(idElt, champ_dwStart24, val, false);
        val.wValue = numChannel;
        sf2->set(idElt, champ_wChannel, val, false);
        val.dwValue = son.get(champ_dwLength);
        sf2->set(idElt, champ_dwLength, val, false);
        val.dwValue = son.get(champ_dwSampleRate);
        sf2->set(idElt, champ_dwSampleRate, val, false);
        val.dwValue = son.get(champ_dwStartLoop);
        sf2->set(idElt, champ_dwStartLoop, val, false);
        val.dwValue = son.get(champ_dwEndLoop);
        sf2->set(idElt, champ_dwEndLoop, val, false);
        val.bValue = (quint8)son.get(champ_byOriginalPitch);
        sf2->set(idElt, champ_byOriginalPitch, val, false);
        val.cValue = (char)son.get(champ_chPitchCorrection);
        sf2->set(idElt, champ_chPitchCorrection, val, false);

        // Chargement dans la ram
        if (Config::getInstance()->getRam())
        {
            val.wValue = 1;
            sf2->set(idElt, champ_ram, val, false);
        }
    }

    return sampleIndex;
}

void GroupeParametres::adaptOffsets(int startLoop, int endLoop, int length)
{
    for (int i = 0; i < _listeParam.size(); i++)
    {
        if (_listeParam.at(i).getOpCode() == Parametre::op_loop_start)
            _listeParam[i].setIntValue(_listeParam.at(i).getIntValue() - startLoop);
        else if (_listeParam.at(i).getOpCode() == Parametre::op_loop_end)
            _listeParam[i].setIntValue(_listeParam.at(i).getIntValue() - endLoop);
        else if (_listeParam.at(i).getOpCode() == Parametre::op_end)
            _listeParam[i].setIntValue(_listeParam.at(i).getIntValue() - length);
    }
    if (startLoop != endLoop && !isDefined(Parametre::op_loop_mode) && (startLoop != 0 || endLoop != 1))
        _listeParam << Parametre("loop_mode", "loop_continuous");
}

void GroupeParametres::adjustStereoVolumeAndCorrection(QString path, int defaultCorrection)
{
    QString sample = getStrValue(Parametre::op_sample).replace("\\", "/");
    if (!sample.isEmpty())
    {
        Sound son(path + "/" + sample);
        if (son.get(champ_wChannels) == 2)
            adjustVolume(3.);
        int correctionSample = son.get(champ_chPitchCorrection);
        if (correctionSample != 0)
            adjustCorrection(correctionSample, defaultCorrection);
    }
}

bool GroupeParametres::sampleValid(QString path) const
{
    bool bRet = false;
    if (this->isDefined(Parametre::op_sample))
    {
        QString sample = getStrValue(Parametre::op_sample).replace("\\", "/");
        bRet = QFile(path + "/" + sample).exists();
    }
    return bRet;
}

void GroupeParametres::checkFilter()
{
    bool removeFilter = false;
    if (isDefined(Parametre::op_filterType))
    {
        QString type = getStrValue(Parametre::op_filterType);
        if (type != "lpf_2p" && type != "lpf_1p") // 1 pôle : acceptable
            removeFilter = true;
    }

    if (removeFilter)
    {
        for (int i = _listeParam.size() - 1; i >= 0; i--)
        {
            if (_listeParam.at(i).getOpCode() == Parametre::op_filterFreq ||
                    _listeParam.at(i).getOpCode() == Parametre::op_filterType ||
                    _listeParam.at(i).getOpCode() == Parametre::op_filterQ)
                _listeParam.removeAt(i);
        }
    }
}

void GroupeParametres::adjustVolume(double offset)
{
    bool isDefined = false;
    for (int i = 0; i < _listeParam.size(); i++)
    {
        if (_listeParam.at(i).getOpCode() == Parametre::op_volume)
        {
            _listeParam[i].setDoubleValue(_listeParam.at(i).getDoubleValue() - offset);
            isDefined = true;
        }
    }
    if (!isDefined)
        _listeParam << Parametre(Parametre::op_volume, -offset);
}

void GroupeParametres::adjustCorrection(int offset, int defaultCorrection)
{
    bool isDefined = false;
    for (int i = 0; i < _listeParam.size(); i++)
    {
        if (_listeParam.at(i).getOpCode() == Parametre::op_tuningFine)
        {
            _listeParam[i].setIntValue(_listeParam.at(i).getIntValue() - offset);
            isDefined = true;
        }
    }
    if (!isDefined)
        _listeParam << Parametre(Parametre::op_tuningFine, defaultCorrection - offset);
}

QStringList GroupeParametres::getFullPath(QString base, QStringList directories)
{
    QStringList listRet;
    if (directories.isEmpty())
        listRet << base;
    else
    {
        QDir dir(base);
        dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);
        QStringList childs = dir.entryList();
        QString nextDirectory = directories.takeFirst();
        for (int i = 0; i < childs.size(); i++)
            if (childs.at(i).compare(nextDirectory, Qt::CaseInsensitive) == 0)
                listRet << getFullPath(base + "/" + childs.at(i), directories);
    }
    return listRet;
}

void GroupeParametres::decode(Pile_sf2 * sf2, EltID idElt) const
{
    Valeur val;
    double dTmp;
    for (int i = 0; i < _listeParam.size(); i++)
    {
        switch (_listeParam.at(i).getOpCode())
        {
        case Parametre::op_key:
            val.rValue.byLo = _listeParam.at(i).getIntValue();
            val.rValue.byHi = val.rValue.byLo;
            sf2->set(idElt, champ_keyRange, val, false);
            val.wValue = _listeParam.at(i).getIntValue();
            sf2->set(idElt, champ_overridingRootKey, val, false);
            break;
        case Parametre::op_keyMin:
            if (sf2->isSet(idElt, champ_keyRange))
                val = sf2->get(idElt, champ_keyRange);
            else
                val.rValue.byHi = 127;
            val.rValue.byLo = _listeParam.at(i).getIntValue();
            sf2->set(idElt, champ_keyRange, val, false);
            break;
        case Parametre::op_keyMax:
            if (sf2->isSet(idElt, champ_keyRange))
                val = sf2->get(idElt, champ_keyRange);
            else
                val.rValue.byLo = 0;
            val.rValue.byHi = _listeParam.at(i).getIntValue();
            sf2->set(idElt, champ_keyRange, val, false);
            break;
        case Parametre::op_velMin:
            if (sf2->isSet(idElt, champ_velRange))
                val = sf2->get(idElt, champ_velRange);
            else
                val.rValue.byHi = 127;
            val.rValue.byLo = _listeParam.at(i).getIntValue();
            sf2->set(idElt, champ_velRange, val, false);
            break;
        case Parametre::op_velMax:
            if (sf2->isSet(idElt, champ_velRange))
                val = sf2->get(idElt, champ_velRange);
            else
                val.rValue.byLo = 0;
            val.rValue.byHi = _listeParam.at(i).getIntValue();
            sf2->set(idElt, champ_velRange, val, false);
            break;
        case Parametre::op_rootKey:
            if (!isDefined(Parametre::op_key))
            {
                val.wValue = _listeParam.at(i).getIntValue();
                sf2->set(idElt, champ_overridingRootKey, val, false);
            }
            break;
        case Parametre::op_exclusiveClass:
            val.wValue = _listeParam.at(i).getIntValue();
            if (isDefined(Parametre::op_off_by) && getIntValue(Parametre::op_off_by) == val.wValue)
                sf2->set(idElt, champ_exclusiveClass, val, false);
            break;
        case Parametre::op_tuningFine:
            val.wValue = _listeParam.at(i).getIntValue();
            sf2->set(idElt, champ_fineTune, val, false);
            break;
        case Parametre::op_tuningCoarse:
            val.wValue = _listeParam.at(i).getIntValue();
            sf2->set(idElt, champ_coarseTune, val, false);
            break;
        case Parametre::op_delay:
            dTmp = _listeParam.at(i).getDoubleValue();
            addSeconds(dTmp, champ_delayModEnv, sf2, idElt);
            addSeconds(dTmp, champ_delayModLFO, sf2, idElt);
            addSeconds(dTmp, champ_delayVibLFO, sf2, idElt);
            addSeconds(dTmp, champ_delayVolEnv, sf2, idElt);
            break;
        case Parametre::op_offset:
            if (idElt.typeElement == elementInstSmpl)
            {
                val.shValue = _listeParam.at(i).getIntValue() % 32768;
                sf2->set(idElt, champ_startAddrsOffset, val, false);
                val.shValue = _listeParam.at(i).getIntValue() / 32768;
                sf2->set(idElt, champ_startAddrsCoarseOffset, val, false);
            }
            break;
        case Parametre::op_end:
            if (idElt.typeElement == elementInstSmpl)
            {
                val.shValue = _listeParam.at(i).getIntValue() % 32768;
                sf2->set(idElt, champ_endAddrsOffset, val, false);
                val.shValue = _listeParam.at(i).getIntValue() / 32768;
                sf2->set(idElt, champ_endAddrsCoarseOffset, val, false);
            }
            break;
        case Parametre::op_loop_start:
            if (idElt.typeElement == elementInstSmpl)
            {
                val.shValue = _listeParam.at(i).getIntValue() % 32768;
                sf2->set(idElt, champ_startloopAddrsOffset, val, false);
                val.shValue = _listeParam.at(i).getIntValue() / 32768;
                sf2->set(idElt, champ_startloopAddrsCoarseOffset, val, false);
            }
            break;
        case Parametre::op_loop_end:
            if (idElt.typeElement == elementInstSmpl)
            {
                val.shValue = _listeParam.at(i).getIntValue() % 32768;
                sf2->set(idElt, champ_endloopAddrsOffset, val, false);
                val.shValue = _listeParam.at(i).getIntValue() / 32768;
                sf2->set(idElt, champ_endloopAddrsCoarseOffset, val, false);
            }
            break;
        case Parametre::op_loop_mode:
            if (_listeParam.at(i).getStringValue() == "no_loop")
                val.wValue = 0;
            else if (_listeParam.at(i).getStringValue() == "one_shot")
            {
                val.wValue = 0;
                addSeconds(100, champ_releaseVolEnv, sf2, idElt);
                addSeconds(100, champ_releaseModEnv, sf2, idElt);
            }
            else if (_listeParam.at(i).getStringValue() == "loop_continuous")
                val.wValue = 1;
            else if (_listeParam.at(i).getStringValue() == "loop_sustain")
                val.wValue = 3;
            sf2->set(idElt, champ_sampleModes, val, false);
            break;
        case Parametre::op_volume:
            dTmp = -_listeParam.at(i).getDoubleValue() / DB_SF2_TO_SFZ;
            if (sf2->isSet(idElt, champ_initialAttenuation))
                dTmp += (double)sf2->get(idElt, champ_initialAttenuation).shValue / 10.;
            val.shValue = qRound(10. * dTmp);
            sf2->set(idElt, champ_initialAttenuation, val, false);
            break;
        case Parametre::op_tuningScale:
            val.shValue = _listeParam.at(i).getIntValue();
            sf2->set(idElt, champ_scaleTuning, val, false);
            break;
        case Parametre::op_ampeg_delay:
            addSeconds(_listeParam.at(i).getDoubleValue(), champ_delayVolEnv, sf2, idElt);
            break;
        case Parametre::op_ampeg_attack:
            val.shValue = qRound(log2m1200(_listeParam.at(i).getDoubleValue()));
            sf2->set(idElt, champ_attackVolEnv, val, false);
            break;
        case Parametre::op_ampeg_hold:
            if (!isDefined(Parametre::op_noteToVolEnvHold))
            {
                val.shValue = qRound(log2m1200(_listeParam.at(i).getDoubleValue()));
                sf2->set(idElt, champ_holdVolEnv, val, false);
            }
            break;
        case Parametre::op_ampeg_decay:
            if (!isDefined(Parametre::op_noteToVolEnvDecay))
            {
                val.shValue = qRound(log2m1200(_listeParam.at(i).getDoubleValue()));
                sf2->set(idElt, champ_decayVolEnv, val, false);
            }
            break;
        case Parametre::op_ampeg_sustain:
            dTmp = _listeParam.at(i).getDoubleValue();
            if (dTmp >= 0.1)
                val.shValue = qRound(-10. * percentToDB(dTmp));
            else
                val.shValue = 1440;
            sf2->set(idElt, champ_sustainVolEnv, val, false);
            break;
        case Parametre::op_ampeg_release:
            val.shValue = qRound(log2m1200(_listeParam.at(i).getDoubleValue()));
            sf2->set(idElt, champ_releaseVolEnv, val, false);
            break;
        case Parametre::op_noteToVolEnvHold:
        {
            double baseValue = 0.001;
            int keynum = 0;
            getKeynumValues(baseValue, keynum, Parametre::op_noteToVolEnvHold, Parametre::op_ampeg_hold);
            val.shValue = keynum;
            sf2->set(idElt, champ_keynumToVolEnvHold, val, false);
            val.shValue = qRound(log2m1200(baseValue));
            sf2->set(idElt, champ_holdVolEnv, val, false);
        }break;
        case Parametre::op_noteToVolEnvDecay:
        {
            double baseValue = 0.001;
            int keynum = 0;
            getKeynumValues(baseValue, keynum, Parametre::op_noteToVolEnvDecay, Parametre::op_ampeg_decay);
            val.shValue = keynum;
            sf2->set(idElt, champ_keynumToVolEnvDecay, val, false);
            val.shValue = qRound(log2m1200(baseValue));
            sf2->set(idElt, champ_decayVolEnv, val, false);
        }break;
        case Parametre::op_reverb:
            val.wValue = qRound(10. * _listeParam.at(i).getDoubleValue());
            sf2->set(idElt, champ_reverbEffectsSend, val, false);
            break;
        case Parametre::op_chorus:
            val.wValue = qRound(10. * _listeParam.at(i).getDoubleValue());
            sf2->set(idElt, champ_chorusEffectsSend, val, false);
            break;
        case Parametre::op_filterFreq:
            val.shValue = qRound(log2m1200(_listeParam.at(i).getDoubleValue() / 8.176));
            sf2->set(idElt, champ_initialFilterFc, val, false);
            break;
        case Parametre::op_filterQ:
            val.shValue = qRound(10. * _listeParam.at(i).getDoubleValue());
            sf2->set(idElt, champ_initialFilterQ, val, false);
            break;
        case Parametre::op_vibLFOdelay:
            if (isDefined(Parametre::op_vibLFOtoTon))
                addSeconds(_listeParam.at(i).getDoubleValue(), champ_delayVibLFO, sf2, idElt);
            break;
        case Parametre::op_vibLFOfreq:
            val.shValue = qRound(log2m1200(_listeParam.at(i).getDoubleValue() / 8.176));
            sf2->set(idElt, champ_freqVibLFO, val, false);
            break;
        case Parametre::op_vibLFOtoTon:
            val.shValue = _listeParam.at(i).getIntValue();
            sf2->set(idElt, champ_vibLfoToPitch, val, false);
            break;
        case Parametre::op_pitcheg_delay:
            if (isDefined(Parametre::op_modEnvToTon))
                addSeconds(_listeParam.at(i).getDoubleValue(), champ_delayModEnv, sf2, idElt);
            break;
        case Parametre::op_pitcheg_attack:
            val.shValue = qRound(log2m1200(_listeParam.at(i).getDoubleValue()));
            sf2->set(idElt, champ_attackModEnv, val, false);
            break;
        case Parametre::op_pitcheg_hold:
            if (!isDefined(Parametre::op_noteToModEnvHold))
            {
                val.shValue = qRound(log2m1200(_listeParam.at(i).getDoubleValue()));
                sf2->set(idElt, champ_holdModEnv, val, false);
            }
            break;
        case Parametre::op_pitcheg_decay:
            if (!isDefined(Parametre::op_noteToModEnvDecay))
            {
                val.shValue = qRound(log2m1200(_listeParam.at(i).getDoubleValue()));
                sf2->set(idElt, champ_decayModEnv, val, false);
            }
            break;
        case Parametre::op_pitcheg_sustain:
            val.shValue = qRound(1000. - 10. * _listeParam.at(i).getDoubleValue());
            sf2->set(idElt, champ_sustainModEnv, val, false);
            break;
        case Parametre::op_pitcheg_release:
            val.shValue = qRound(log2m1200(_listeParam.at(i).getDoubleValue()));
            sf2->set(idElt, champ_releaseModEnv, val, false);
            break;
        case Parametre::op_modEnvToTon:
            val.shValue = _listeParam.at(i).getIntValue();
            sf2->set(idElt, champ_modEnvToPitch, val, false);
            break;
        case Parametre::op_noteToModEnvHold:
        {
            double baseValue = 0.001;
            int keynum = 0;
            getKeynumValues(baseValue, keynum, Parametre::op_noteToModEnvHold, Parametre::op_pitcheg_hold);
            val.shValue = keynum;
            sf2->set(idElt, champ_keynumToModEnvHold, val, false);
            val.shValue = qRound(log2m1200(baseValue));
            sf2->set(idElt, champ_holdModEnv, val, false);
        }break;
        case Parametre::op_noteToModEnvDecay:
        {
            double baseValue = 0.001;
            int keynum = 0;
            getKeynumValues(baseValue, keynum, Parametre::op_noteToModEnvDecay, Parametre::op_pitcheg_decay);
            val.shValue = keynum;
            sf2->set(idElt, champ_keynumToModEnvDecay, val, false);
            val.shValue = qRound(log2m1200(baseValue));
            sf2->set(idElt, champ_decayModEnv, val, false);
        }break;
        case Parametre::op_modLFOdelay:
            addSeconds(_listeParam.at(i).getDoubleValue(), champ_delayModLFO, sf2, idElt);
            break;
        case Parametre::op_modLFOfreq:
            val.shValue = qRound(log2m1200(_listeParam.at(i).getDoubleValue() / 8.176));
            sf2->set(idElt, champ_freqModLFO, val, false);
            break;
        case Parametre::op_modLFOtoVolume:
            val.shValue = qRound(10. * _listeParam.at(i).getDoubleValue());
            sf2->set(idElt, champ_modLfoToVolume, val, false);
            break;
        default:
            break;
        }
    }

    // Vélocité fixe
    if (isDefined(Parametre::op_amp_velcurve_1) && isDefined(Parametre::op_amp_velcurve_127))
    {
        int velMin = qRound(127. * getDoubleValue(Parametre::op_amp_velcurve_1));
        int velMax = qRound(127. * getDoubleValue(Parametre::op_amp_velcurve_127));
        if (velMin == velMax)
        {
            val.wValue = velMin;
            sf2->set(idElt, champ_velocity, val, false);
        }
    }

    // Enveloppe pour le filtre
    if (isDefined(Parametre::op_modEnvToFilter))
    {
        if (isDefined(Parametre::op_modEnvToTon))
        {
            // Mêmes paramètres ?
            if (getDoubleValue(Parametre::op_fileg_delay) == getDoubleValue(Parametre::op_pitcheg_delay) &&
                getDoubleValue(Parametre::op_fileg_attack) == getDoubleValue(Parametre::op_pitcheg_attack) &&
                getDoubleValue(Parametre::op_fileg_hold) == getDoubleValue(Parametre::op_pitcheg_hold) &&
                getDoubleValue(Parametre::op_fileg_decay) == getDoubleValue(Parametre::op_pitcheg_decay) &&
                getDoubleValue(Parametre::op_fileg_sustain) == getDoubleValue(Parametre::op_pitcheg_sustain) &&
                getDoubleValue(Parametre::op_fileg_release) == getDoubleValue(Parametre::op_pitcheg_release) &&
                getDoubleValue(Parametre::op_fileg_holdcc133) == getDoubleValue(Parametre::op_noteToModEnvHold) &&
                getDoubleValue(Parametre::op_fileg_decaycc133) == getDoubleValue(Parametre::op_noteToModEnvDecay))
            {
                val.shValue = getIntValue(Parametre::op_modEnvToFilter);
                sf2->set(idElt, champ_modEnvToFilterFc, val, false);
            }
        }
        else
        {
            if (isDefined(Parametre::op_fileg_delay))
                addSeconds(getDoubleValue(Parametre::op_fileg_delay), champ_delayModEnv, sf2, idElt);
            if (isDefined(Parametre::op_fileg_attack))
            {
                val.shValue = qRound(log2m1200(getDoubleValue(Parametre::op_fileg_attack)));
                sf2->set(idElt, champ_attackModEnv, val, false);
            }
            if (isDefined(Parametre::op_fileg_hold) && !isDefined(Parametre::op_fileg_holdcc133))
            {
                val.shValue = qRound(log2m1200(getDoubleValue(Parametre::op_fileg_hold)));
                sf2->set(idElt, champ_holdModEnv, val, false);
            }
            if (isDefined(Parametre::op_fileg_decay) && !isDefined(Parametre::op_fileg_decaycc133))
            {
                val.shValue = qRound(log2m1200(getDoubleValue(Parametre::op_fileg_decay)));
                sf2->set(idElt, champ_decayModEnv, val, false);
            }
            if (isDefined(Parametre::op_fileg_sustain))
            {
                val.shValue = qRound(1000. - 10. * getDoubleValue(Parametre::op_fileg_sustain));
                sf2->set(idElt, champ_sustainModEnv, val, false);
            }
            if (isDefined(Parametre::op_fileg_release))
            {
                val.shValue = qRound(log2m1200(getDoubleValue(Parametre::op_fileg_release)));
                sf2->set(idElt, champ_releaseModEnv, val, false);
            }
            if (isDefined(Parametre::op_fileg_holdcc133))
            {
                double baseValue = 0.001;
                int keynum = 0;
                getKeynumValues(baseValue, keynum, Parametre::op_fileg_holdcc133, Parametre::op_fileg_hold);
                val.shValue = keynum;
                sf2->set(idElt, champ_keynumToModEnvHold, val, false);
                val.shValue = qRound(log2m1200(baseValue));
                sf2->set(idElt, champ_holdModEnv, val, false);
            }
            if (isDefined(Parametre::op_fileg_decaycc133))
            {
                double baseValue = 0.001;
                int keynum = 0;
                getKeynumValues(baseValue, keynum, Parametre::op_fileg_decaycc133, Parametre::op_fileg_decay);
                val.shValue = keynum;
                sf2->set(idElt, champ_keynumToModEnvDecay, val, false);
                val.shValue = qRound(log2m1200(baseValue));
                sf2->set(idElt, champ_decayModEnv, val, false);
            }

            val.shValue = getIntValue(Parametre::op_modEnvToFilter);
            sf2->set(idElt, champ_modEnvToFilterFc, val, false);
        }
    }

    // LFO filtre
    if (isDefined(Parametre::op_modLFOtoFilter))
    {
        if (isDefined(Parametre::op_modLFOtoVolume))
        {
            // Mêmes paramètres ?
            if (getDoubleValue(Parametre::op_filLFOdelay) == getDoubleValue(Parametre::op_modLFOdelay) &&
                getDoubleValue(Parametre::op_filLFOfreq) == getDoubleValue(Parametre::op_modLFOfreq))
            {
                val.shValue = getIntValue(Parametre::op_modLFOtoFilter);
                sf2->set(idElt, champ_modLfoToFilterFc, val, false);
            }
        }
        else
        {
            if (isDefined(Parametre::op_filLFOdelay))
                addSeconds(getDoubleValue(Parametre::op_filLFOdelay), champ_delayModLFO, sf2, idElt);
            if (isDefined(Parametre::op_filLFOfreq))
            {
                val.shValue = qRound(log2m1200(getDoubleValue(Parametre::op_filLFOfreq) / 8.176));
                sf2->set(idElt, champ_freqModLFO, val, false);
            }
            val.shValue = getIntValue(Parametre::op_modLFOtoFilter);
            sf2->set(idElt, champ_modLfoToFilterFc, val, false);
        }
    }
}

void GroupeParametres::getKeynumValues(double &baseValue, int &keynum,
                                       Parametre::OpCode opCodeKeynum, Parametre::OpCode opCodeBase) const
{
    // Etendue de la division
    int noteMin = 0;
    int noteMax = 127;
    if (isDefined(Parametre::op_key))
        noteMin = noteMax = getIntValue(Parametre::op_key);
    if (isDefined(Parametre::op_keyMin))
        noteMin = getIntValue(Parametre::op_keyMin);
    if (isDefined(Parametre::op_keyMax))
        noteMax = getIntValue(Parametre::op_keyMax);

    // Valeur de base du paramètre
    double valBase = 0.001;
    if (isDefined(opCodeBase))
        valBase = getDoubleValue(opCodeBase);

    // Valeur aux positions min et max
    double valKeynum = getDoubleValue(opCodeKeynum);
    double valMin = valBase + (double)noteMin / 127. * valKeynum;
    double valMax = valBase + (double)noteMax / 127. * valKeynum;

    if (noteMin == noteMax || valMax <= 0 || valMin <= 0)
    {
        baseValue = valMin;
        keynum = 0;
    }
    else
    {
        keynum = qRound(1200. * log2(valMin / valMax) / (noteMax - noteMin));
        baseValue = valMin * qPow(valMin / valMax, (double)(noteMin - 60) / (noteMax - noteMin));
    }
}

void GroupeParametres::addSeconds(double value, Champ champ, Pile_sf2 * sf2, EltID id)
{
    double dTmp;
    Valeur val;
    if (sf2->isSet(id, champ))
        dTmp = d1200e2(sf2->get(id, champ).shValue);
    else
        dTmp = 0;
    val.shValue = qRound(log2m1200(dTmp + value));
    sf2->set(id, champ, val, false);
}


Parametre::Parametre(QString opcode, QString valeur) :
    _opcode(op_unknown),
    _intValue(0),
    _dblValue(0.)
{
    QString valeurLow = valeur.toLower();
    opcode = opcode.remove('_');
    if (opcode == "sample")
    {
        _opcode = op_sample;
        _strValue = valeur;
    }
    else if (opcode == "key")
    {
        _opcode = op_key;
        _intValue = Config::getInstance()->getKeyNum(valeurLow, true);
    }
    else if (opcode == "lokey")
    {
        _opcode = op_keyMin;
        _intValue = Config::getInstance()->getKeyNum(valeurLow, true);
    }
    else if (opcode == "hikey")
    {
        _opcode = op_keyMax;
        _intValue = Config::getInstance()->getKeyNum(valeurLow, true);
    }
    else if (opcode == "lovel")
    {
        _opcode = op_velMin;
        _intValue = valeurLow.toInt();
    }
    else if (opcode == "hivel")
    {
        _opcode = op_velMax;
        _intValue = valeurLow.toInt();
    }
    else if (opcode == "pitchkeycenter")
    {
        _opcode = op_rootKey;
        _intValue = Config::getInstance()->getKeyNum(valeurLow, true);
    }
    else if (opcode == "group")
    {
        _opcode = op_exclusiveClass;
        _intValue = valeurLow.toInt();
    }
    else if (opcode == "offby")
    {
        _opcode = op_off_by;
        _intValue = valeurLow.toInt();
    }
    else if (opcode == "tune")
    {
        _opcode = op_tuningFine;
        _intValue = valeurLow.toInt();
    }
    else if (opcode == "transpose")
    {
        _opcode = op_tuningCoarse;
        _intValue = valeurLow.toInt();
    }
    else if (opcode == "delay")
    {
        _opcode = op_delay;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "offset")
    {
        _opcode = op_offset;
        _intValue = valeurLow.toInt();
    }
    else if (opcode == "end")
    {
        _opcode = op_end;
        _intValue = valeurLow.toInt() + 1;
    }
    else if (opcode == "loopstart")
    {
        _opcode = op_loop_start;
        _intValue = valeurLow.toInt();
    }
    else if (opcode == "loopend")
    {
        _opcode = op_loop_end;
        _intValue = valeurLow.toInt() + 1;
    }
    else if (opcode == "loopmode")
    {
        _opcode = op_loop_mode;
        _strValue = valeurLow;
    }
    else if (opcode == "pan")
    {
        _opcode = op_pan;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "width")
    {
        _opcode = op_width;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "position")
    {
        _opcode = op_position;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "volume")
    {
        _opcode = op_volume;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "pitchkeytrack")
    {
        _opcode = op_tuningScale;
        _intValue = valeurLow.toInt();
    }
    else if (opcode == "ampegdelay")
    {
        _opcode = op_ampeg_delay;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "ampegattack")
    {
        _opcode = op_ampeg_attack;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "ampeghold")
    {
        _opcode = op_ampeg_hold;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "ampegdecay")
    {
        _opcode = op_ampeg_decay;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "ampegsustain")
    {
        _opcode = op_ampeg_sustain;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "ampegrelease")
    {
        _opcode = op_ampeg_release;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "ampegholdcc133")
    {
        _opcode = op_noteToVolEnvHold;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "ampegdecaycc133")
    {
        _opcode = op_noteToVolEnvDecay;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "effect1")
    {
        _opcode = op_reverb;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "effect2")
    {
        _opcode = op_chorus;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "filtype")
    {
        _opcode = op_filterType;
        _strValue = valeurLow;
    }
    else if (opcode == "cutoff")
    {
        _opcode = op_filterFreq;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "resonance")
    {
        _opcode = op_filterQ;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "pitchlfodelay")
    {
        _opcode = op_vibLFOdelay;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "pitchlfofreq")
    {
        _opcode = op_vibLFOfreq;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "pitchlfodepth")
    {
        _opcode = op_vibLFOtoTon;
        _intValue = valeurLow.toInt();
    }
    else if (opcode == "pitchegdelay")
    {
        _opcode = op_pitcheg_delay;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "pitchegattack")
    {
        _opcode = op_pitcheg_attack;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "pitcheghold")
    {
        _opcode = op_pitcheg_hold;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "pitchegdecay")
    {
        _opcode = op_pitcheg_decay;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "pitchegsustain")
    {
        _opcode = op_pitcheg_sustain;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "pitchegrelease")
    {
        _opcode = op_pitcheg_release;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "pitchegdepth")
    {
        _opcode = op_modEnvToTon;
        _intValue = valeurLow.toInt();
    }
    else if (opcode == "pitchegholdcc133")
    {
        _opcode = op_noteToModEnvHold;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "pitchegdecaycc133")
    {
        _opcode = op_noteToModEnvDecay;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "amplfodelay")
    {
        _opcode = op_modLFOdelay;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "amplfofreq")
    {
        _opcode = op_modLFOfreq;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "amplfodepth")
    {
        _opcode = op_modLFOtoVolume;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "ampvelcurve1")
    {
        _opcode = op_amp_velcurve_1;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "ampvelcurve127")
    {
        _opcode = op_amp_velcurve_127;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "filegdelay")
    {
        _opcode = op_fileg_delay;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "filegattack")
    {
        _opcode = op_fileg_attack;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "fileghold")
    {
        _opcode = op_fileg_hold;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "filegdecay")
    {
        _opcode = op_fileg_decay;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "filegsustain")
    {
        _opcode = op_fileg_sustain;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "filegrelease")
    {
        _opcode = op_fileg_release;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "filegdepth")
    {
        _opcode = op_modEnvToFilter;
        _intValue = valeurLow.toInt();
    }
    else if (opcode == "filegholdcc133")
    {
        _opcode = op_fileg_holdcc133;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "filegdecaycc133")
    {
        _opcode = op_fileg_decaycc133;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "fillfodelay")
    {
        _opcode = op_filLFOdelay;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "fillfofreq")
    {
        _opcode = op_filLFOfreq;
        _dblValue = valeurLow.toDouble();
    }
    else if (opcode == "fillfodepth")
    {
        _opcode = op_modLFOtoFilter;
        _intValue = valeurLow.toInt();
    }
    else if (opcode == "lochan")
    {
        _opcode = op_chanMin;
        _intValue = valeurLow.toInt();
    }
    else if (opcode == "hichan")
    {
        _opcode = op_chanMax;
        _intValue = valeurLow.toInt();
    }
    else
        qDebug() << "non pris en charge: " + opcode + " (" + valeur + ")";
}

QString GroupeParametres::getName(QString name, int maxCharacters, int suffixNumber, QString suffix)
{
    int suffixSize = suffix.size();

    // Cas où la taille du suffix est supérieure au nombre de caractères max
    if (suffixSize > maxCharacters)
        return name.left(maxCharacters);

    // Cas où un numéro n'est pas nécessaire
    if (suffixNumber == 0)
        return name.left(maxCharacters - suffixSize) + suffix;

    QString suffixNum = QString::number(suffixNumber);
    int suffixNumSize = suffixNum.length() + 1;

    // Cas où le suffix numérique est trop grand
    if (suffixNumSize > 3 || maxCharacters - suffixSize < suffixNumSize)
        return name.left(maxCharacters - suffixSize) + suffix;

    return name.left(maxCharacters - suffixNumSize - suffixSize) + suffix + "-" + suffixNum;
}
