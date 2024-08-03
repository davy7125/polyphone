/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2024 Davy Triponney                                **
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
**  Website/Contact: https://www.polyphone.io                             **
**             Date: 01.01.2013                                           **
***************************************************************************/

#include "inputparsersfz.h"
#include "soundfontmanager.h"
#include <QRegularExpression>

InputParserSfz::InputParserSfz() : AbstractInputParser() {}

void InputParserSfz::processInternal(QString fileName, SoundfontManager * sm, bool &success, QString &error, int &sf2Index, QString &tempFilePath)
{
    Q_UNUSED(sm)
    Q_UNUSED(tempFilePath)

    _currentBloc = BLOC_UNKNOWN;

    // Parse the file
    _rootDir = QFileInfo(fileName).dir().path();
    if (_rootDir.endsWith('/'))
        _rootDir = _rootDir.left(_rootDir.size() - 1);
    parseFile(fileName, success, error);
    if (!success)
        return;

    bool isChannel10 = true;
    double ampliMax = 0;
    for (int i = 0; i < _presetList.size(); i++)
    {
        // All samples must be valid
        _presetList[i].moveOpcodesInGlobal(_globalZone);
        _presetList[i].moveOpcodeInSamples(SfzParameter::op_sample, QMetaType::QString);
        _presetList[i].checkSampleValid(QFileInfo(fileName).path());

        // Offsets must be defined for samples, not in gloobal division
        _presetList[i].moveOpcodeInSamples(SfzParameter::op_offset, QMetaType::Int);
        _presetList[i].moveOpcodeInSamples(SfzParameter::op_end, QMetaType::Int);
        _presetList[i].moveOpcodeInSamples(SfzParameter::op_loop_start, QMetaType::Int);
        _presetList[i].moveOpcodeInSamples(SfzParameter::op_loop_end, QMetaType::Int);
        _presetList[i].moveOpcodeInSamples(SfzParameter::op_loop_mode, QMetaType::QString);

        // Same for pan, samples, off_by et group
        _presetList[i].moveOpcodeInSamples(SfzParameter::op_pan, QMetaType::Double);
        _presetList[i].moveOpcodeInSamples(SfzParameter::op_width, QMetaType::Double);
        _presetList[i].moveOpcodeInSamples(SfzParameter::op_position, QMetaType::Double);
        _presetList[i].moveOpcodeInSamples(SfzParameter::op_off_by, QMetaType::Int);
        _presetList[i].moveOpcodeInSamples(SfzParameter::op_exclusiveClass, QMetaType::Int);
        _presetList[i].moveModInSamples();
        _presetList[i].moveKeynumInSamples(SfzParameter::op_noteToVolEnvDecay, SfzParameter::op_ampeg_decay);
        _presetList[i].moveKeynumInSamples(SfzParameter::op_noteToVolEnvHold, SfzParameter::op_ampeg_hold);
        _presetList[i].moveKeynumInSamples(SfzParameter::op_noteToModEnvDecay, SfzParameter::op_pitcheg_decay);
        _presetList[i].moveKeynumInSamples(SfzParameter::op_noteToModEnvHold, SfzParameter::op_pitcheg_hold);
        _presetList[i].moveKeynumInSamples(SfzParameter::op_fileg_decaycc133, SfzParameter::op_fileg_decay);
        _presetList[i].moveKeynumInSamples(SfzParameter::op_fileg_holdcc133, SfzParameter::op_fileg_hold);
        _presetList[i].adjustCorrection(QFileInfo(fileName).path());

        // Adapt the volume if a volume modulation applies
        _presetList[i].adjustModulationVolume();

        // Process / reject filters
        _presetList[i].checkFilter();
        _presetList[i].checkKeyTrackedFilter();

        // Maximum amplification
        ampliMax = qMax(ampliMax, _presetList[i].getAmpliMax());

        // Search canal 10
        isChannel10 &= _presetList[i].isChannel10();
    }

    // Adapt the volume if the max amplification is > 0
    if (ampliMax > 0)
        for (int i = 0; i < _presetList.size(); i++)
            _presetList[i].adjustVolume(-ampliMax);

    // Create a soundfont
    createSf2(sf2Index, fileName, isChannel10);

    success = true;
}

void InputParserSfz::parseFile(QString filename, bool &success, QString &error)
{
    if (_openFilePaths.contains(filename))
    {
        // Recursion!
        success = false;
        error = tr("File recursion") + " (#include)";
        return;
    }

    QFile inputFile(filename);
    if (!inputFile.open(QIODevice::ReadOnly))
    {
        success = false;
        error = tr("Cannot open file \"%1\"").arg(filename);
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

        // Cut the line
        QStringList list = line.split(" ", Qt::KeepEmptyParts);
        int length = list.size();
        for (int i = length - 1; i >= 1; i--)
        {
            if (!list.at(i).contains(QRegularExpression("[<>=]")))
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
            // Valid?
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
                QStringList splitTmp = str.split(' ', Qt::SkipEmptyParts);
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

QString InputParserSfz::getFilePathFromInclude(QString str)
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

QString InputParserSfz::applyReplacements(QString opcodeValue)
{
    foreach (QString key, _replacements.keys())
        opcodeValue = opcodeValue.replace(key, _replacements[key]);
    return opcodeValue;
}

void InputParserSfz::changeBloc(QString bloc)
{
    if (bloc == "control")
        _currentBloc = BLOC_CONTROL;
    else if (bloc == "global")
        _currentBloc = BLOC_GLOBAL;
    else if (bloc == "group")
    {
        _currentBloc = BLOC_GROUP;
        _presetList << SfzParameterGroup();
    }
    else if (bloc == "region")
    {
        _currentBloc = BLOC_REGION;
        if (_presetList.isEmpty())
            _presetList << SfzParameterGroup();
        _presetList.last().newGroup();
    }
    else
    {
        qWarning() << "unknown block" << bloc;
        _currentBloc = BLOC_UNKNOWN;
    }
}

void InputParserSfz::addOpcode(QString opcode, QString value)
{
    switch (_currentBloc)
    {
    case BLOC_GROUP: case BLOC_REGION:
        _presetList.last().addParam(opcode, value);
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

void InputParserSfz::createSf2(int &sf2Index, QString filename, bool isChannel10)
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
        sm->set(idSf2, champ_name, tr("Sfz import"));
    }
    sm->set(idSf2, champ_ICMT, QString("Sf2 imported from sfz by Polyphone"));
    sm->closestAvailablePreset(idSf2, numBank, numPreset);

    // Create a preset
    EltID idPrst = idSf2;
    idPrst.typeElement = elementPrst;
    idPrst.indexElt = sm->add(idPrst);
    sm->set(idPrst, champ_name, nom);
    AttributeValue val;
    val.wValue = numBank;
    sm->set(idPrst, champ_wBank, val);
    val.wValue = numPreset;
    sm->set(idPrst, champ_wPreset, val);

    // Create instruments
    EltID idInst = idSf2;
    idInst.typeElement = elementInst;
    for (int i = 0; i < _presetList.size(); i++)
    {
        idInst.indexElt = sm->add(idInst);
        QString nomInst = _presetList[i].getLabel();
        if (nomInst == "")
        {
            if (_presetList.size() > 9)
                nomInst = nom.left(17) + "-" + QString::number(i + 1);
            else if (_presetList.size() > 1)
                nomInst = nom.left(18) + "-" + QString::number(i + 1);
            else
                nomInst = nom;
        }
        sm->set(idInst, champ_name, nomInst);

        EltID idInstSmpl = idInst;
        idInstSmpl.typeElement = elementInstSmpl;

        // Link to the preset
        EltID idPrstInst = idPrst;
        idPrstInst.typeElement = elementPrstInst;
        idPrstInst.indexElt2 = sm->add(idPrstInst);
        val.dwValue = idInst.indexElt;
        sm->set(idPrstInst, champ_instrument, val);

        // Fill the instrument and create samples
        _presetList[i].decode(sm, idInst, QFileInfo(filename).path());

        // Find the preset keyRange and velRange
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

        // Simplify parameters in instruments
        sm->simplify(idInst, champ_initialAttenuation);
        sm->simplify(idInst, champ_fineTune);
        sm->simplify(idInst, champ_sampleModes);
        sm->simplify(idInst, champ_exclusiveClass);
        sm->simplify(idInst, champ_releaseVolEnv);

        // Delete keyrange and velocity range of the global division in the instrument
        sm->reset(idInst, champ_velRange);
        sm->reset(idInst, champ_keyRange);
    }

    // Possible 24-bit mode
    EltID idSmpl(elementSmpl, sf2Index);
    foreach (int i, sm->getSiblings(idSmpl))
    {
        idSmpl.indexElt = i;
        if (sm->get(idSmpl, champ_bpsFile).wValue >= 24)
        {
            AttributeValue val;
            val.wValue = 24;
            sm->set(idSf2, champ_wBpsSave, val);
            break;
        }
    }
}

QString InputParserSfz::getInstrumentName(QString filePath, int &numBank, int &numPreset)
{
    QFileInfo fileInfo(filePath);
    QString nomFichier = fileInfo.completeBaseName();
    QString nomDir = fileInfo.dir().dirName();

    // Preset number
    QRegularExpression regExp("^\\d\\d\\d.*");
    if (regExp.match(nomFichier).hasMatch())
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

    // Bank number
    if (regExp.match(nomDir).hasMatch())
    {
        numBank = nomDir.left(3).toInt();
        if (numBank < 0 || numBank > 127)
            numBank = 0;
    }

    // Instrument name
    if (nomFichier.isEmpty())
        nomFichier = tr("untitled");
    return nomFichier.left(20);
}
