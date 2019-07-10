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

#include "inputsfz.h"
#include "soundfontmanager.h"

InputSfz::InputSfz() : AbstractInput() {}

void InputSfz::processInternal(QString fileName, SoundfontManager * sm, bool &success, QString &error, int &sf2Index, QString &tempFilePath)
{
    Q_UNUSED(sm)
    Q_UNUSED(tempFilePath)
    Q_UNUSED(error)

    _currentBloc = BLOC_UNKNOWN;

    // Parse the file
    _rootDir = QFileInfo(fileName).dir().path();
    if (_rootDir.endsWith('/'))
        _rootDir = _rootDir.left(_rootDir.size() - 1);
    parseFile(fileName, success, error);
    if (!success)
        return;

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
        _listeEnsembles[i].moveOpcodesInGlobal(_globalZone);
        _listeEnsembles[i].moveOpcodeInSamples(SfzParameter::op_sample, QVariant::String);
        _listeEnsembles[i].checkSampleValid(QFileInfo(fileName).path());
        _listeEnsembles[i].moveOpcodeInSamples(SfzParameter::op_offset, QVariant::Int);
        _listeEnsembles[i].moveOpcodeInSamples(SfzParameter::op_end, QVariant::Int);
        _listeEnsembles[i].moveOpcodeInSamples(SfzParameter::op_loop_start, QVariant::Int);
        _listeEnsembles[i].moveOpcodeInSamples(SfzParameter::op_loop_end, QVariant::Int);
        _listeEnsembles[i].moveOpcodeInSamples(SfzParameter::op_loop_mode, QVariant::String);
        _listeEnsembles[i].moveOpcodeInSamples(SfzParameter::op_pan, QVariant::Double);
        _listeEnsembles[i].moveOpcodeInSamples(SfzParameter::op_off_by, QVariant::Int);
        _listeEnsembles[i].moveOpcodeInSamples(SfzParameter::op_exclusiveClass, QVariant::Int);
        _listeEnsembles[i].moveModInSamples();
        _listeEnsembles[i].moveKeynumInSamples(SfzParameter::op_noteToVolEnvDecay, SfzParameter::op_ampeg_decay);
        _listeEnsembles[i].moveKeynumInSamples(SfzParameter::op_noteToVolEnvHold, SfzParameter::op_ampeg_hold);
        _listeEnsembles[i].moveKeynumInSamples(SfzParameter::op_noteToModEnvDecay, SfzParameter::op_pitcheg_decay);
        _listeEnsembles[i].moveKeynumInSamples(SfzParameter::op_noteToModEnvHold, SfzParameter::op_pitcheg_hold);
        _listeEnsembles[i].moveKeynumInSamples(SfzParameter::op_fileg_decaycc133, SfzParameter::op_fileg_decay);
        _listeEnsembles[i].moveKeynumInSamples(SfzParameter::op_fileg_holdcc133, SfzParameter::op_fileg_hold);
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
    createSf2(sf2Index, fileName, isChannel10);

    success = true;
}

void InputSfz::parseFile(QString filename, bool &success, QString &error)
{
    if (_openFilePaths.contains(filename))
    {
        // Recursion!
        success = false;
        error = trUtf8("File recursion") + " (#include)";
        return;
    }

    QFile inputFile(filename);
    if (!inputFile.open(QIODevice::ReadOnly))
    {
        success = false;
        error = trUtf8("Cannot open file \"%1\"").arg(filename);
        return;
    }

    _openFilePaths << filename;
    QTextStream in(&inputFile);
    while (!in.atEnd())
    {
        // Read one line
        QString line = in.readLine();
        if (line.isEmpty())
            continue;

        // Remove comment
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

        // Trim spaces and remove empty fields
        length = list.size();
        for (int i = length - 1; i >= 0; i--)
        {
            QString strTmp = list.at(i).trimmed();
            if (strTmp.isEmpty())
                list.removeAt(i);
            else
                list[i] = strTmp;
        }

        // Store elements
        foreach (QString str, list)
        {
            // Valide ?
            if (str.size() <= 2)
                continue;

            if (str.indexOf("#include") == 0)
            {
                // Read another file
                QString otherFile = getFilePathFromInclude(str);
                this->parseFile(otherFile, success, error);

                // In case of error, return immediately
                if (!success)
                {
                    inputFile.close();
                    return;
                }
            }
            else if (str.indexOf("#define") == 0)
            {
                QStringList splitTmp = str.split(' ', QString::SkipEmptyParts);
                if (splitTmp.size() == 3)
                    _replacements[splitTmp[1]] = splitTmp[2];
            }
            else if (str.left(1) == "<" && str.right(1) == ">")
                changeBloc(str.right(str.size()-1).left(str.size()-2));
            else if (str.contains("="))
            {
                int index = str.indexOf("=");
                QString opcode = str.left(index).toLower();
                QString value = this->applyReplacements(str.right(str.length() - index - 1));
                if (!opcode.isEmpty() && !value.isEmpty())
                    addOpcode(opcode, value);
            }
        }
    }
    inputFile.close();
    _openFilePaths.removeAll(filename);

    success = true;
}

QString InputSfz::getFilePathFromInclude(QString str)
{
    // Skip the "#include", remove quotes and extra spaces
    str = str.right(str.size() - 8).replace("\"", "").replace("'", "").trimmed();
    if (str.isEmpty())
        return "";

    // Make an absolute path
    str = str.replace("\\", "/");
    if (str[0] == '/')
        str = _rootDir + str;
    else
        str = _rootDir + "/" + str;

    return str;
}

QString InputSfz::applyReplacements(QString opcodeValue)
{
    foreach (QString key, _replacements.keys())
        opcodeValue = opcodeValue.replace(key, _replacements[key]);
    return opcodeValue;
}

void InputSfz::changeBloc(QString bloc)
{
    if (bloc == "control")
        _currentBloc = BLOC_CONTROL;
    else if (bloc == "global")
        _currentBloc = BLOC_GLOBAL;
    else if (bloc == "group")
    {
        _currentBloc = BLOC_GROUP;
        _listeEnsembles << SfzParameterGroupAssembly();
    }
    else if (bloc == "region")
    {
        _currentBloc = BLOC_REGION;
        if (_listeEnsembles.isEmpty())
            _listeEnsembles << SfzParameterGroupAssembly();
        _listeEnsembles.last().newGroup();
    }
    else
    {
        qWarning() << "unknown block" << bloc;
        _currentBloc = BLOC_UNKNOWN;
    }
}

void InputSfz::addOpcode(QString opcode, QString value)
{
    switch (_currentBloc)
    {
    case BLOC_GROUP: case BLOC_REGION:
        _listeEnsembles.last().addParam(opcode, value);
        break;
    case BLOC_CONTROL:
        if (opcode == "default_path")
        {
            QString defaultPath = value.replace("\\", "/");
            if (!defaultPath.isEmpty() && defaultPath[0] == '/')
                defaultPath = defaultPath.right(defaultPath.size() - 1);
            if (!defaultPath.isEmpty() && defaultPath.endsWith('/'))
                defaultPath = defaultPath.left(defaultPath.size() - 1);
            SfzParameter::DEFAULT_PATH = defaultPath;
        }
        break;
    case BLOC_GLOBAL:
        _globalZone << SfzParameter(opcode, value);
        break;
    case BLOC_UNKNOWN:
        break;
    }
}

void InputSfz::createSf2(int &sf2Index, QString filename, bool isChannel10)
{
    SoundfontManager * sm = SoundfontManager::getInstance();
    int numBank = 0;
    int numPreset = 0;
    QString nom = getInstrumentName(filename, numBank, numPreset);
    if (isChannel10)
        numBank = 128;
    EltID idSf2(elementSf2, -1, -1, -1, -1);
    if (sf2Index == -1)
    {
        idSf2.indexSf2 = sm->add(idSf2);
        sf2Index = idSf2.indexSf2;
        sm->set(idSf2, champ_name, nom);
    }
    else
    {
        idSf2.indexSf2 = sf2Index;
        sm->set(idSf2, champ_name, trUtf8("Sfz import"));
    }
    sm->set(idSf2, champ_ICMT, QString("Sf2 imported from sfz by Polyphone"));
    sm->closestAvailablePreset(idSf2, numBank, numPreset);

    // Création d'un preset
    EltID idPrst = idSf2;
    idPrst.typeElement = elementPrst;
    idPrst.indexElt = sm->add(idPrst);
    sm->set(idPrst, champ_name, nom);
    AttributeValue val;
    val.wValue = numBank;
    sm->set(idPrst, champ_wBank, val);
    val.wValue = numPreset;
    sm->set(idPrst, champ_wPreset, val);

    // Création des instruments
    EltID idInst = idSf2;
    idInst.typeElement = elementInst;
    for (int i = 0; i < _listeEnsembles.size(); i++)
    {
        idInst.indexElt = sm->add(idInst);
        QString nomInst = _listeEnsembles[i].getLabel();
        if (nomInst == "")
        {
            if (_listeEnsembles.size() > 9)
                nomInst = nom.left(17) + "-" + QString::number(i+1);
            else if (_listeEnsembles.size() > 1)
                nomInst = nom.left(18) + "-" + QString::number(i+1);
            else
                nomInst = nom;
        }
        sm->set(idInst, champ_name, nomInst);

        EltID idInstSmpl = idInst;
        idInstSmpl.typeElement = elementInstSmpl;

        // Lien dans le preset
        EltID idPrstInst = idPrst;
        idPrstInst.typeElement = elementPrstInst;
        idPrstInst.indexElt2 = sm->add(idPrstInst);
        val.dwValue = idInst.indexElt;
        sm->set(idPrstInst, champ_instrument, val);

        // Remplissage de l'instrument et création des samples
        _listeEnsembles[i].decode(sm, idInst, QFileInfo(filename).path());

        // Détermination keyRange et velRange du preset
        int keyMin = 127;
        int keyMax = 0;
        int velMin = 127;
        int velMax = 0;
        foreach (int j, sm->getSiblings(idInstSmpl))
        {
            idInstSmpl.indexElt2 = j;
            if (sm->isSet(idInstSmpl, champ_keyRange))
            {
                RangesType range = sm->get(idInstSmpl, champ_keyRange).rValue;
                keyMin = qMin(keyMin, (int)range.byLo);
                keyMax = qMax(keyMax, (int)range.byHi);
            }
            if (sm->isSet(idInstSmpl, champ_velRange))
            {
                RangesType range = sm->get(idInstSmpl, champ_velRange).rValue;
                velMin = qMin(velMin, (int)range.byLo);
                velMax = qMax(velMax, (int)range.byHi);
            }
        }
        if (sm->isSet(idInst, champ_keyRange))
        {
            RangesType range = sm->get(idInst, champ_keyRange).rValue;
            keyMin = qMin(keyMin, (int)range.byLo);
            keyMax = qMax(keyMax, (int)range.byHi);
        }
        if (sm->isSet(idInst, champ_velRange))
        {
            RangesType range = sm->get(idInst, champ_velRange).rValue;
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
        sm->set(idPrstInst, champ_keyRange, val);
        if (velMin <= velMax)
        {
            val.rValue.byLo = velMin;
            val.rValue.byHi = velMax;
            sm->set(idPrstInst, champ_velRange, val);
        }

        // Suppression des atténuations, corrections, type loop et exclusive class inutiles
        if (!sm->isSet(idInst, champ_initialAttenuation))
        {
            foreach (int i, sm->getSiblings(idInstSmpl))
            {
                idInstSmpl.indexElt2 = i;
                if (sm->get(idInstSmpl, champ_initialAttenuation).wValue == 0)
                    sm->reset(idInstSmpl, champ_initialAttenuation);
            }
        }
        if (!sm->isSet(idInst, champ_fineTune))
        {
            foreach (int i, sm->getSiblings(idInstSmpl))
            {
                idInstSmpl.indexElt2 = i;
                if (sm->get(idInstSmpl, champ_fineTune).wValue == 0)
                    sm->reset(idInstSmpl, champ_fineTune);
            }
        }
        int typeLoop = sm->get(idInst, champ_sampleModes).wValue;
        if (typeLoop == 0)
            sm->reset(idInst, champ_sampleModes);
        foreach (int i, sm->getSiblings(idInstSmpl))
        {
            idInstSmpl.indexElt2 = i;
            if (sm->get(idInstSmpl, champ_sampleModes).wValue == typeLoop)
                sm->reset(idInstSmpl, champ_sampleModes);
        }
        if (!sm->isSet(idInst, champ_exclusiveClass))
        {
            foreach (int i, sm->getSiblings(idInstSmpl))
            {
                idInstSmpl.indexElt2 = i;
                if (sm->get(idInstSmpl, champ_exclusiveClass).wValue == 0)
                    sm->reset(idInstSmpl, champ_exclusiveClass);
            }
        }

        // Suppression keyrange et velocity range de la division globale de l'instrument
        sm->reset(idInst, champ_velRange);
        sm->reset(idInst, champ_keyRange);
    }
}

QString InputSfz::getInstrumentName(QString filePath, int &numBank, int &numPreset)
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
        nomFichier = trUtf8("untitled");
    return nomFichier.left(20);
}
