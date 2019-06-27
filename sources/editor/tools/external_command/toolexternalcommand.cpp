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

#include "toolexternalcommand.h"
#include "toolexternalcommand_gui.h"
#include "toolexternalcommand_parameters.h"
#include <QProgressDialog>
#include <QTemporaryFile>
#include <QDir>
#include <QApplication>
#include "soundfontmanager.h"
#include "samplewriterwav.h"
#include <QProcess>

ToolExternalCommand::ToolExternalCommand() :
    AbstractToolIterating(elementSmpl, new ToolExternalCommand_parameters(), new ToolExternalCommand_gui())
{
    _async = false;
}

void ToolExternalCommand::beforeProcess(IdList ids)
{
    Q_UNUSED(ids)
    _warning = "";
}

void ToolExternalCommand::process(SoundfontManager * sm, EltID id, AbstractToolParameters * parameters)
{
    // Check that the ID has not been already processed
    if (_processedIds.contains(id))
        return;

    // Parameters
    ToolExternalCommand_parameters * params = (ToolExternalCommand_parameters *)parameters;
    if (params->getCommandHistory().empty())
        return;
    QString command = params->getCommandHistory()[0];
    bool stereo = params->getStereo();
    bool replaceInfo = params->getReplaceInfo();

    // Prepare the command
    QStringList arguments = command.split(QRegExp(" +(?=(?:[^\"]*\"[^\"]*\")*[^\"]*$)"));
    if (arguments.count() < 2)
    {
        _warning = "invalid command";
        return; // Shouldn't happen
    }
    QString program = arguments.takeFirst().replace("\"", "");
    int wavPosition = arguments.indexOf("{wav}");
    if (wavPosition == -1)
    {
        _warning = "missing part '{wav}'";
        return; // Shouldn't happen
    }

    // IDs to process
    EltID id2(elementSmpl, id.indexSf2, -1);
    _processedIds << id;
    if (stereo)
    {
        SFSampleLink type = sm->get(id, champ_sfSampleType).sfLinkValue;
        if (type != monoSample && type != RomMonoSample)
        {
            // Find the other part of the sound
            id2.indexElt = sm->get(id, champ_wSampleLink).wValue;
            if (sm->isValid(id2) && !_processedIds.contains(id2))
                _processedIds << id2;
            else
                id2.indexElt = -1;
        }
    }

    // Export the sample in a temporary file
    QTemporaryFile * tempFile = new QTemporaryFile(QDir::tempPath() + "/" + QApplication::applicationName() + "-XXXXXX.wav");
    tempFile->setAutoRemove(true);
    tempFile->open();
    QString pathTempFile = tempFile->fileName();
    tempFile->close();

    SampleWriterWav writer(pathTempFile);
    if (id2.indexElt != -1)
        writer.write(sm->getSound(id), sm->getSound(id2));
    else
        writer.write(sm->getSound(id));

    // Execute an external command
#ifdef Q_OS_WIN
    arguments[wavPosition] = pathTempFile.replace('/', '\\');
#else
    arguments[wavPosition] = pathTempFile;
#endif

    switch (QProcess::execute(program, arguments))
    {
    case -2:
        _warning = trUtf8("Couldn't start the command.");
        break;
    case -1:
        _warning = trUtf8("The execution of the command ended with an error.");
        break;
    default:
    {
        // Import the sample
        Sound sound(pathTempFile, false);
        AttributeValue val;
        val.wValue = 0;
        sound.set(champ_wChannel, val);
        import(id, sound, sm, replaceInfo);
        if (id2.indexSf2 != -1 && sound.getUInt32(champ_wChannels) == 2)
        {
            val.wValue = 1;
            sound.set(champ_wChannel, val);
            import(id2, sound, sm, replaceInfo);
        }
    }
        break;
    }

    // Delete the temp file
    delete tempFile;
}

void ToolExternalCommand::import(EltID id, Sound &sound, SoundfontManager * sm, bool replaceInfo)
{
    sm->set(id, champ_sampleDataFull24, sound.getData(24));

    AttributeValue val;
    val.dwValue = sound.getUInt32(champ_dwStart16);
    sm->set(id, champ_dwStart16, val);
    val.dwValue = sound.getUInt32(champ_dwStart24);
    sm->set(id, champ_dwStart24, val);
    val.dwValue = sound.getUInt32(champ_dwLength);
    sm->set(id, champ_dwLength, val);
    val.dwValue = sound.getUInt32(champ_dwSampleRate);
    sm->set(id, champ_dwSampleRate, val);

    // Sample configuration
    if (replaceInfo)
    {
        // Loop
        val.dwValue = sound.getUInt32(champ_dwEndLoop);
        if (val.dwValue != 0)
        {
            sm->set(id, champ_dwEndLoop, val);
            val.dwValue = sound.getUInt32(champ_dwStartLoop);
            sm->set(id, champ_dwStartLoop, val);
        }

        // Original pitch and correction
        if (sound.getUInt32(champ_pitchDefined) == 1)
        {
            val.bValue = (quint8)sound.getUInt32(champ_byOriginalPitch);
            sm->set(id, champ_byOriginalPitch, val);
            val.cValue = (char)sound.getInt32(champ_chPitchCorrection);
            sm->set(id, champ_chPitchCorrection, val);
        }
    }

    // Check that start loop and end loop are not out of range
    if (sm->get(id, champ_dwStartLoop).dwValue > sound.getUInt32(champ_dwLength) ||
            sm->get(id, champ_dwEndLoop).dwValue > sound.getUInt32(champ_dwLength))
    {
        val.dwValue = 0;
        sm->set(id, champ_dwStartLoop, val);
        sm->set(id, champ_dwEndLoop, val);
    }
}

QString ToolExternalCommand::getWarning()
{
    return _warning;
}
