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

#include "inputparsergrandorgue.h"
#include "soundfontmanager.h"
#include "grandorguerank.h"
#include "grandorguestop.h"
#include "grandorgueswitch.h"
#include "grandorguedatathrough.h"
#include "utils.h"

InputParserGrandOrgue::InputParserGrandOrgue() : AbstractInputParser(),
    _godt(new GrandOrgueDataThrough())
{

}

InputParserGrandOrgue::~InputParserGrandOrgue()
{
    while (!_ranks.empty())
        delete _ranks.take(_ranks.firstKey());
    while (!_stops.empty())
        delete _stops.take(_stops.firstKey());
    while (!_switches.empty())
        delete _switches.take(_switches.firstKey());
    delete _godt;
}

void InputParserGrandOrgue::processInternal(QString fileName, SoundfontManager * sm, bool &success, QString &error, int &sf2Index, QString &tempFilePath)
{
    Q_UNUSED(sm)
    Q_UNUSED(tempFilePath)

    // Initialization
    _currentSection = SECTION_UNKNOWN;
    _currentIndex = -1;
    _ranks.clear();
    _stops.clear();
    _switches.clear();

    // Parse the file
    _rootDir = QFileInfo(fileName).dir().path();
    if (_rootDir.endsWith('/'))
        _rootDir = _rootDir.left(_rootDir.size() - 1);
    parseFile(fileName, success, error);
    if (!success)
        return;

    // Create a sf2
    createSf2(sf2Index, fileName);
    success = true;
}

void InputParserGrandOrgue::parseFile(QString filename, bool &success, QString &error)
{
    // Open the file
    QFile inputFile(filename);
    if (!inputFile.open(QIODevice::ReadOnly))
    {
        success = false;
        error = tr("Cannot open file \"%1\"").arg(filename);
        return;
    }

    bool utf8 = Utils::isValidUtf8(inputFile.readAll());
    inputFile.seek(0);

    QTextStream in(&inputFile);
#if QT_VERSION < 0x060000
    in.setCodec(utf8 ? "UTF-8" : "latin1");
#else
    in.setEncoding(utf8 ? QStringConverter::Utf8 : QStringConverter::Latin1);
#endif
    while (!in.atEnd())
    {
        // Read one line, skip empty lines or comments
        QString line = in.readLine();

        // Remove comments
        int tmp;
        if ((tmp = line.indexOf(';')) != -1)
        {
            line = line.left(tmp);
        }

        // Trim the line and skip it if empty
        line = line.trimmed();
        if (line.isEmpty())
            continue;

        if (line.startsWith("[") && line.endsWith("]"))
        {
            // New section
            startSection(line.mid(1, line.length() - 2).toLower());
        }
        else
        {
            // Process data
            tmp = line.indexOf('=');
            QString key = line.left(tmp).trimmed().toLower();
            QString value = line.right(line.length() - tmp - 1).trimmed().replace('\\', '/');
            if (!key.isEmpty() && !value.isEmpty())
                processData(key, value);
        }
    }
    inputFile.close();

    success = true;
}

void InputParserGrandOrgue::startSection(QString sectionName)
{
    if (sectionName.startsWith("rank"))
    {
        bool ok = false;
        _currentIndex = sectionName.right(sectionName.length() - 4).toInt(&ok);
        if (!ok)
        {
            _currentSection = SECTION_UNKNOWN;
            _currentIndex = -1;
        }
        else
            _currentSection = SECTION_RANK;
    }
    else if (sectionName.startsWith("stop"))
    {
        bool ok = false;
        _currentIndex = sectionName.right(sectionName.length() - 4).toInt(&ok);
        if (!ok)
        {
            _currentSection = SECTION_UNKNOWN;
            _currentIndex = -1;
        }
        else
            _currentSection = SECTION_STOP;
    }
    else if (sectionName.startsWith("switch"))
    {
        bool ok = false;
        _currentIndex = sectionName.right(sectionName.length() - 6).toInt(&ok);
        if (!ok)
        {
            _currentSection = SECTION_UNKNOWN;
            _currentIndex = -1;
        }
        else
            _currentSection = SECTION_SWITCH;
    }
    else if (sectionName == "organ")
    {
        _currentSection = SECTION_ORGAN;
        _currentIndex = -1;
    }
    else
        _currentSection = SECTION_UNKNOWN;
}

void InputParserGrandOrgue::processData(QString key, QString value)
{
    switch (_currentSection)
    {
    case SECTION_RANK:
        if (_currentIndex != -1)
        {
            if (!_ranks.contains(_currentIndex))
                _ranks[_currentIndex] = new GrandOrgueRank(_rootDir, _godt, _currentIndex);
            _ranks[_currentIndex]->readData(key, value);
        }
        break;
    case SECTION_STOP:
        if (_currentIndex != -1)
        {
            if (!_stops.contains(_currentIndex))
                _stops[_currentIndex] = new GrandOrgueStop(_rootDir, _godt, _currentIndex);
            _stops[_currentIndex]->readData(key, value);
        }
        break;
    case SECTION_SWITCH:
        if (_currentIndex != -1)
        {
            if (!_switches.contains(_currentIndex))
                _switches[_currentIndex] = new GrandOrgueSwitch(_godt, _currentIndex);
            _switches[_currentIndex]->readData(key, value);
        }
        break;
    case SECTION_ORGAN:
        _organProperties[key] = value;
        break;
    case SECTION_UNKNOWN:
        // Nothing
        break;
    }
}

void InputParserGrandOrgue::createSf2(int &sf2Index, QString filename)
{
    // Create a new soundfont, store the sf2 index
    SoundfontManager * sm = SoundfontManager::getInstance();
    EltID idSf2(elementSf2, -1, -1, -1, -1);
    idSf2.indexSf2 = sm->add(idSf2);
    sf2Index = idSf2.indexSf2;

    // Title, comment
    QFileInfo fileInfo(filename);
    sm->set(idSf2, champ_name, fileInfo.completeBaseName());
    sm->set(idSf2, champ_ICMT, this->getComment());

    // Pre-process everything
    foreach (GrandOrgueRank * rank, _ranks)
        rank->preProcess();
    foreach (GrandOrgueStop * stop, _stops)
        stop->preProcess();
    foreach (GrandOrgueSwitch * goSwitch, _switches)
        goSwitch->preProcess();
    _godt->finalizePreprocess();

    // Process switches and stops for creating presets and instruments
    foreach (GrandOrgueSwitch * goSwitch, _switches)
        goSwitch->process(sm, sf2Index, _stops, _ranks);
    _godt->useNextBank();
    foreach (GrandOrgueStop * stop, _stops)
        stop->process(sm, sf2Index, _ranks);
}

QString InputParserGrandOrgue::getComment()
{
    QString comment = "";
    if (_organProperties.contains("churchname"))
        comment = "Church name: " + _organProperties["churchname"];
    if (_organProperties.contains("churchaddress"))
        comment += (comment.isEmpty() ? QString("") : "\n") +
                "Church address: " + _organProperties["churchaddress"];
    if (_organProperties.contains("organbuilder"))
        comment += (comment.isEmpty() ? QString("") : "\n") +
                "Organ builder: " + _organProperties["organbuilder"];
    if (_organProperties.contains("organbuilddate"))
        comment += (comment.isEmpty() ? QString("") : "\n") +
                "Organ build date: " + _organProperties["organbuilddate"];
    if (_organProperties.contains("organcomments"))
        comment += (comment.isEmpty() ? QString("") : "\n") +
                "Organ comments: " + _organProperties["organcomments"];
    if (_organProperties.contains("recordingdetails"))
        comment += (comment.isEmpty() ? QString("") : "\n") +
                "Recording details: " + _organProperties["recordingdetails"];
    return comment;
}
