#include "external_command/toolexternalcommand.h"
#include "external_command/toolexternalcommand_gui.h"
#include "external_command/toolexternalcommand_parameters.h"
#include <QProgressDialog>
#include <QTemporaryFile>
#include <QDir>
#include <QApplication>
#include "soundfontmanager.h"

ToolExternalCommand::ToolExternalCommand() : AbstractToolIterating(elementSmpl, new ToolExternalCommand_parameters(), new ToolExternalCommand_gui(), false),
    _process(NULL)
{

}

ToolExternalCommand::~ToolExternalCommand()
{
    if (_process != NULL)
        _process->kill();
}

void ToolExternalCommand::process(SoundfontManager * sm, EltID id, AbstractToolParameters * parameters)
{
    _sm = sm;

    // Check that the ID has not been already processed
    if (_processedIds.contains(id))
        return;

    // Parameters
    ToolExternalCommand_parameters * params = (ToolExternalCommand_parameters *)parameters;
    QString command = params->getCommand();
    bool stereo = params->getStereo();
    _replaceInfo = params->getReplaceInfo();

    // Prepare the command
    QStringList arguments = command.split(QRegExp(" +(?=(?:[^\"]*\"[^\"]*\")*[^\"]*$)"));
    if (arguments.count() < 2 || arguments.indexOf("{wav}") == -1)
        return; // Shouldn't happen

    // IDs to process
    _processedIds << id;
    EltID id2(elementSmpl, id.indexSf2, -1);
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

    // Program to call, argument to replace
    _program = _arguments.takeFirst().replace("\"", "");
    _indexWav = _arguments.indexOf("{wav}");

    // Export the sample in a temporary file
    QTemporaryFile * tempFile = new QTemporaryFile(QDir::tempPath() + "/" + QApplication::applicationName() + "-XXXXXX.wav");
    tempFile->open();
    _pathTempFile = tempFile->fileName();
    tempFile->close();
    if (id2.indexElt != -1)
        Sound::exporter(_pathTempFile, sm->getSon(id), sm->getSon(id2));
    else
        Sound::exporter(_pathTempFile, sm->getSon(id));

#ifdef Q_OS_WIN
    _arguments[_indexWav] = _pathTempFile.replace('/', '\\');
#else
    _arguments[_indexWav] = _pathTempFile;
#endif

    // Definitely close the file
    tempFile->setAutoRemove(false);
    delete tempFile;

    // Start a new process
    _process = new QProcess(this);
    connect(_process, SIGNAL(stateChanged(QProcess::ProcessState)), this, SLOT(onProcessStateChanged(QProcess::ProcessState)));
    _process->start(_program, _arguments);
}

void ToolExternalCommand::onProcessStateChanged(QProcess::ProcessState state)
{
    if (state != QProcess::NotRunning)
        return;

    // Delete the process that has just been used
    _process->deleteLater();
    _process = NULL;

    // Import the sample
    Sound sound(_pathTempFile, false);
    Valeur val;
    val.wValue = 0;
    sound.set(champ_wChannel, val);
    import(_id1, sound);
    if (_id2.indexSf2 != -1 && sound.get(champ_wChannels) == 2)
    {
        val.wValue = 1;
        sound.set(champ_wChannel, val);
        import(_id2, sound);
    }
}

void ToolExternalCommand::import(EltID id, Sound &sound)
{
    _sm->set(id, champ_sampleDataFull24, sound.getData(24));

    Valeur val;
    val.dwValue = sound.get(champ_dwStart16);
    _sm->set(id, champ_dwStart16, val);
    val.dwValue = sound.get(champ_dwStart24);
    _sm->set(id, champ_dwStart24, val);
    val.dwValue = sound.get(champ_dwLength);
    _sm->set(id, champ_dwLength, val);
    val.dwValue = sound.get(champ_dwSampleRate);
    _sm->set(id, champ_dwSampleRate, val);

    // Sample configuration
    if (_replaceInfo)
    {
        // Loop
        val.dwValue = sound.get(champ_dwEndLoop);
        if (val.dwValue != 0)
        {
            _sm->set(id, champ_dwEndLoop, val);
            val.dwValue = sound.get(champ_dwStartLoop);
            _sm->set(id, champ_dwStartLoop, val);
        }

        // Original pitch and correction
        if (sound.get(champ_pitchDefined) == 1)
        {
            val.bValue = (quint8)sound.get(champ_byOriginalPitch);
            _sm->set(id, champ_byOriginalPitch, val);
            val.cValue = (char)sound.get(champ_chPitchCorrection);
            _sm->set(id, champ_chPitchCorrection, val);
        }
    }

    // Check that start loop and end loop are not out of range
    if (_sm->get(id, champ_dwStartLoop).dwValue > sound.get(champ_dwLength) ||
        _sm->get(id, champ_dwEndLoop).dwValue > sound.get(champ_dwLength))
    {
        val.dwValue = 0;
        _sm->set(id, champ_dwStartLoop, val);
        _sm->set(id, champ_dwEndLoop, val);
    }
}
