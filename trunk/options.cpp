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

#include "options.h"
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QCoreApplication>

Options::Options(int argc, char *argv[]) :
    _currentState(STATE_INPUT_FILE), // By default args are input files
    _mode(MODE_GUI),
    _error(false),
    _sfzPresetPrefix(false),
    _sfzOneDirPerBank(false),
    _sfzGeneralMidi(false)
{
    _appPath = QFileInfo(QCoreApplication::applicationFilePath()).path();

    // Convert into QStringList
    QStringList args;
    for (int i = 1; i < argc; i++) // The first argument is rejected
        args << QString(argv[i]);

    // Argument processing
    foreach (QString arg, args)
    {
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
    case 'z':
        _currentState = STATE_SFZ_OPTIONS;
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
    case STATE_SFZ_OPTIONS:
        if (arg.count() == 3)
        {
            if (arg[0] == '0' || arg[0] == '1')
                _sfzPresetPrefix = (arg[0] == '1');
            else
                _error = true;

            if (arg[1] == '0' || arg[1] == '1')
                _sfzOneDirPerBank = (arg[1] == '1');
            else
                _error = true;

            if (arg[2] == '0' || arg[2] == '1')
                _sfzGeneralMidi = (arg[2] == '1');
            else
                _error = true;
        }
        else
            _error = true;
        break;
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
        QString extension = QFileInfo(inputFile).suffix().toLower();
        if (extension != "sf2" && extension != "sf3" && extension != "sfark" && extension != "sfz")
        {
            _error = true;
            return;
        }
    }

    switch (_mode)
    {
    case MODE_GUI:
        if (_outputDirectory != "" || _outputFile != "")
            _error = true;
        break;
    case MODE_CONVERSION_TO_SF2:
        if (_inputFiles.count() != 1 || QFileInfo(_outputFile).suffix().toLower() != "sf2")
            _error = true;
        break;
    case MODE_CONVERSION_TO_SF3:
        if (_inputFiles.count() != 1 || QFileInfo(_outputFile).suffix().toLower() != "sf3")
            _error = true;
        break;
    case MODE_CONVERSION_TO_SFZ:
        if (_inputFiles.count() != 1)
            _error = true;
        break;
    }
}

void Options::postTreatment()
{
    // Complete the path of input files
    for (int i = 0; i < _inputFiles.count(); i++)
        if (_inputFiles[i][0] != '/')
            _inputFiles[i] = _appPath + "/" + _inputFiles[i];

    if (_mode != MODE_GUI && _outputDirectory == "")
    {
        // The output directory is the same than those containing the input file
        _outputDirectory = QFileInfo(_inputFiles[0]).dir().path();
    }
}

QString Options::getOutputFileFullPath()
{
    QString strTmp = _outputDirectory;
    if (!strTmp.endsWith('/'))
        strTmp += '/';
    return strTmp + _outputFile;
}
