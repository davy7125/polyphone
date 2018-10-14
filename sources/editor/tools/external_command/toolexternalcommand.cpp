#include "toolexternalcommand.h"
#include "toolexternalcommand_gui.h"
#include "toolexternalcommand_parameters.h"
#include <QProgressDialog>
#include <QTemporaryFile>
#include <QDir>
#include <QApplication>
#include "soundfontmanager.h"
#include <QProcess>

ToolExternalCommand::ToolExternalCommand() : AbstractToolIterating(elementSmpl, new ToolExternalCommand_parameters(), new ToolExternalCommand_gui(), false)
{

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
    if (id2.indexElt != -1)
        Sound::exporter(pathTempFile, sm->getSon(id), sm->getSon(id2));
    else
        Sound::exporter(pathTempFile, sm->getSon(id));

    // Execute an external command
#ifdef Q_OS_WIN
    arguments[wavPosition] = pathTempFile.replace('/', '\\');
#else
    arguments[wavPosition] = pathTempFile;
#endif

    switch (QProcess::execute(program, arguments))
    {
    case -2:
        _warning = trUtf8("La commande n'a pas pu démarrer.");
        break;
    case -1:
        _warning = trUtf8("La commande s'est terminée avec une erreur");
        break;
    default:
    {
        // Import the sample
        Sound sound(pathTempFile, false);
        AttributeValue val;
        val.wValue = 0;
        sound.set(champ_wChannel, val);
        import(id, sound, sm, replaceInfo);
        if (id2.indexSf2 != -1 && sound.get(champ_wChannels) == 2)
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
    val.dwValue = sound.get(champ_dwStart16);
    sm->set(id, champ_dwStart16, val);
    val.dwValue = sound.get(champ_dwStart24);
    sm->set(id, champ_dwStart24, val);
    val.dwValue = sound.get(champ_dwLength);
    sm->set(id, champ_dwLength, val);
    val.dwValue = sound.get(champ_dwSampleRate);
    sm->set(id, champ_dwSampleRate, val);

    // Sample configuration
    if (replaceInfo)
    {
        // Loop
        val.dwValue = sound.get(champ_dwEndLoop);
        if (val.dwValue != 0)
        {
            sm->set(id, champ_dwEndLoop, val);
            val.dwValue = sound.get(champ_dwStartLoop);
            sm->set(id, champ_dwStartLoop, val);
        }

        // Original pitch and correction
        if (sound.get(champ_pitchDefined) == 1)
        {
            val.bValue = (quint8)sound.get(champ_byOriginalPitch);
            sm->set(id, champ_byOriginalPitch, val);
            val.cValue = (char)sound.get(champ_chPitchCorrection);
            sm->set(id, champ_chPitchCorrection, val);
        }
    }

    // Check that start loop and end loop are not out of range
    if (sm->get(id, champ_dwStartLoop).dwValue > sound.get(champ_dwLength) ||
            sm->get(id, champ_dwEndLoop).dwValue > sound.get(champ_dwLength))
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
