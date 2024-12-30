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

#include "options.h"
#include <QFileInfo>
#include <QDir>
#include <QCoreApplication>
#include "inputfactory.h"
#include "playeroptions.h"

Options::Options(int argc, char *argv[]) :
    _currentState(STATE_INPUT_FILE), // By default args are input files
    _mode(MODE_GUI),
    _error(false),
    _help(false),
    _sf3Quality(1),
    _sfzPresetPrefix(false),
    _sfzOneDirPerBank(false),
    _sfzGeneralMidi(false),
    _playerOptions(nullptr)
{
    _appPath = QFileInfo(QCoreApplication::applicationFilePath()).path();

    // Convert into QStringList
    QStringList args;
    for (int i = 1; i < argc; i++) // The first argument is rejected (executable path)
#ifdef Q_OS_WIN
        args << QString::fromLatin1(argv[i]);
#else
        args << QString(argv[i]);
#endif

    // Argument processing
    foreach (QString arg, args)
    {
        if (arg.isEmpty())
            continue;

        if (arg[0] == '-')
            processType1(arg);
        else
            processType2(arg);

        if (_error)
            break;
    }

    // Check for errors
    if (!_error)
        checkErrors();

    // Post-treatment
    if (!_error)
        postTreatment();
}

Options::~Options()
{
    delete _playerOptions;
}

void Options::processType1(QString arg)
{
    if (arg.size() != 2)
    {
        _error = true;
        return;
    }

    switch (arg[1].toLatin1())
    {
    case '0':
        _mode = MODE_GUI;
        break;
    case '1':
        _mode = MODE_CONVERSION_TO_SF2;
        break;
    case '2':
        _mode = MODE_CONVERSION_TO_SF3;
        break;
    case '3':
        _mode = MODE_CONVERSION_TO_SFZ;
        break;
    case 'd':
        _currentState = STATE_OUTPUT_DIRECTORY;
        break;
    case 'i':
        _currentState = STATE_INPUT_FILE;
        break;
    case 'o':
        _currentState = STATE_OUTPUT_FILE;
        break;
    case 'c':
        _currentState = STATE_CONFIG;
        break;
    case 'h':
        _help = true;
        break;
    case 'r':
        _mode = MODE_RESET_CONFIG;
        break;
    case 's':
        _mode = MODE_SYNTHESIZER;
        break;
    default:
        _error = true;
        break;
    }
}

void Options::processType2(QString arg)
{
    switch (_currentState)
    {
    case STATE_INPUT_FILE:
        _inputFiles << arg;
        break;
    case STATE_OUTPUT_DIRECTORY:
        _outputDirectory = arg;
        _currentState = STATE_NONE; // no more output
        break;
    case STATE_OUTPUT_FILE:
        _outputFile = arg;
        _currentState = STATE_NONE; // no more output
        break;
    case STATE_CONFIG: {
        QStringList configurations = arg.split('|');

        switch (_mode)
        {
        case MODE_CONVERSION_TO_SFZ:
            if (configurations.count() >= 1)
            {
                if (configurations[0] == "1")
                    _sfzPresetPrefix = true;
                else if (configurations[0] != "0")
                    _error = true;
            }

            if (configurations.count() >= 2)
            {
                if (configurations[1] == "1")
                    _sfzOneDirPerBank = true;
                else if (configurations[1] != "0")
                    _error = true;
            }

            if (configurations.count() >= 3)
            {
                if (configurations[2] == "1")
                    _sfzGeneralMidi = true;
                else if (configurations[2] != "0")
                    _error = true;
            }

            if (configurations.count() > 3)
                _error = true;
            break;
        case  MODE_CONVERSION_TO_SF2:
            if (configurations.count() > 0)
                _error = true;
            break;
        case MODE_CONVERSION_TO_SF3:
            if (configurations.count() >= 1)
            {
                if (configurations[0] == "0")
                    _sf3Quality = 0;
                else if (configurations[0] == "1")
                    _sf3Quality = 1;
                else if (configurations[0] == "2")
                    _sf3Quality = 2;
                else
                    _error = true;
            }

            if (configurations.count() > 1)
                _error = true;
            break;
        case MODE_SYNTHESIZER:
            if (_playerOptions == nullptr)
            {
                _playerOptions = new PlayerOptions();
                _error = !_playerOptions->parse(configurations);
            }
            else
                _error = true;
            break;
        default:
            _error = true;
            break;
        }

    } break;
    default:
        _error = true;
        break;
    }
}

void Options::checkErrors()
{
    // Input files
    foreach (QString inputFile, _inputFiles)
    {
        if (!InputFactory::isSuffixSupported(QFileInfo(inputFile).suffix()))
        {
            _error = true;
            return;
        }
    }

    switch (_mode)
    {
    case MODE_RESET_CONFIG:
        _error = false;
        break;
    case MODE_GUI: case MODE_SYNTHESIZER:
        if (_outputDirectory != "" || _outputFile != "")
            _error = true;
        break;
    case MODE_CONVERSION_TO_SF2: case MODE_CONVERSION_TO_SF3: case MODE_CONVERSION_TO_SFZ:
        if (_inputFiles.count() != 1)
            _error = true;
        break;
    }
}

void Options::postTreatment()
{
    if (_mode == MODE_CONVERSION_TO_SF2 || _mode == MODE_CONVERSION_TO_SF3 || _mode == MODE_CONVERSION_TO_SFZ)
    {
        // By default, the output directory is the same than the input file directory
        if (_outputDirectory == "")
            _outputDirectory = QFileInfo(_inputFiles[0]).dir().absolutePath();

        // By default, the output file name is the same than the input file name
        if (_outputFile == "")
            _outputFile = QFileInfo(_inputFiles[0]).completeBaseName();
    }
}

QString Options::getOutputFileFullPath()
{
    QString strTmp = _outputDirectory;
    if (!strTmp.endsWith('/'))
        strTmp += '/';

    // Extension
    QString extension = "";
    switch (_mode)
    {
    case MODE_CONVERSION_TO_SF2:
        extension = ".sf2";
        break;
    case MODE_CONVERSION_TO_SF3:
        extension = ".sf3";
        break;
    case MODE_CONVERSION_TO_SFZ:
        extension = ".sfz";
        break;
    default:
        break;
    }

    return strTmp + _outputFile + extension;
}

QString Options::getInputFilesAsString()
{
    // All files
    QStringList filesToOpen = _inputFiles;
    filesToOpen.removeAll("");
    QString str = filesToOpen.join('|');

    // Possible add the player options
    if (_playerOptions != nullptr)
        str += "||" + _playerOptions->toString();

    return str;
}
