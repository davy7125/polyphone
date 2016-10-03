#include "externalcommandrunner.h"
#include "sound.h"
#include <QProgressDialog>
#include <QApplication>
#include <QTemporaryFile>

ExternalCommandRunner::ExternalCommandRunner(Pile_sf2 *sf2, QWidget *parent) :
    QObject(parent),
    _sf2(sf2),
    _progress(NULL),
    _process(NULL),
    _tempFile(NULL)
{}

ExternalCommandRunner::~ExternalCommandRunner()
{
    delete _progress;
    if (_process != NULL)
        _process->kill();
}

void ExternalCommandRunner::run(QList<EltID> ids, QString command, bool stereo, bool replaceInfo)
{
    // Prepare the command
    _arguments = command.split(QRegExp(" +(?=(?:[^\"]*\"[^\"]*\")*[^\"]*$)"));
    if (_arguments.count() < 2 || _arguments.indexOf("{wav}") == -1 || ids.isEmpty())
    {
        this->deleteLater();
        return; // Shouldn't happen
    }
    _replaceInfo = replaceInfo;

    // IDs to process
    if (stereo)
        storeStereoIds(ids);
    else
        foreach (EltID id, ids)
            _remainingIds << (QList<EltID>() << id);

    // Program to call, argument to replace and ids to process
    _program = _arguments.takeFirst().replace("\"", "");
    _indexWav = _arguments.indexOf("{wav}");

    // Prepare and show the progress bar
    _progress = new QProgressDialog("", trUtf8("Annuler"), 0, _remainingIds.count(), (QWidget*)this->parent());
    _progress->setCancelButton(NULL);
    _progress->setWindowFlags(_progress->windowFlags() & ~Qt::WindowContextHelpButtonHint & ~Qt::WindowCloseButtonHint);
    _progress->setWindowModality(Qt::WindowModal);
    _progress->setFixedWidth(350);
    _progress->show();

    // Process the first sample
    processOne();
}

void ExternalCommandRunner::storeStereoIds(QList<EltID> ids)
{
    while (!ids.isEmpty())
    {
        QList<EltID> id;
        id << ids.takeFirst();
        SFSampleLink type = _sf2->get(id[0], champ_sfSampleType).sfLinkValue;

        if (type != monoSample && type != RomMonoSample)
        {
            // Find the other part of the sound
            EltID id2 = id[0];
            id2.indexElt = _sf2->get(id[0], champ_wSampleLink).wValue;
            if (_sf2->isValid(id2))
            {
                id << id2;
                ids.removeAll(id2);
            }
        }

        _remainingIds << id;
    }
}

void ExternalCommandRunner::processOne()
{
    // Take the first id in the remaining list
    QList<EltID> ids = _remainingIds.takeFirst();
    _id1 = ids[0];
    if (ids.count() == 2)
        _id2 = ids[1];
    else
        _id2.indexSf2 = -1;

    _progress->setValue(_progress->value() + 1);
    _progress->setLabelText(trUtf8("Traitement ") + _sf2->getQstr(_id1, champ_name) +
                            (ids.count() == 2 ? ", " + _sf2->getQstr(_id2, champ_name) : ""));
    QApplication::processEvents();

    // Export the sample in a temporary file
    _tempFile = new QTemporaryFile(QApplication::applicationName() + "-XXXXXX.wav");
    _tempFile->open();
    _tempFile->close();
    if (ids.count() == 2)
        Sound::exporter(_tempFile->fileName(), _sf2->getSon(_id1), _sf2->getSon(_id2));
    else
        Sound::exporter(_tempFile->fileName(), _sf2->getSon(_id1));
    _arguments[_indexWav] = _tempFile->fileName();

    // Start a new process
    _process = new QProcess(this);
    connect(_process, SIGNAL(stateChanged(QProcess::ProcessState)), this, SLOT(onProcessStateChanged(QProcess::ProcessState)));
    _process->start(_program, _arguments);
}

void ExternalCommandRunner::onProcessStateChanged(QProcess::ProcessState state)
{
    if (state != QProcess::NotRunning)
        return;

    // Delete the process that has just been used
    _process->deleteLater();
    _process = NULL;

    // Import the sample
    Sound sound(_tempFile->fileName(), false);
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

    // Delete the temporary file
    delete _tempFile;
    _tempFile = NULL;

    // Process the new sample if possible or quit
    if (_remainingIds.isEmpty() || _progress->wasCanceled())
    {
        emit(finished());
        this->deleteLater();
    }
    else
        processOne();
}

void ExternalCommandRunner::import(EltID id, Sound &sound)
{
    _sf2->set(id, champ_sampleDataFull24, sound.getData(24));

    Valeur val;
    val.dwValue = sound.get(champ_dwStart16);
    _sf2->set(id, champ_dwStart16, val);
    val.dwValue = sound.get(champ_dwStart24);
    _sf2->set(id, champ_dwStart24, val);
    val.dwValue = sound.get(champ_dwLength);
    _sf2->set(id, champ_dwLength, val);
    val.dwValue = sound.get(champ_dwSampleRate);
    _sf2->set(id, champ_dwSampleRate, val);

    // Sample configuration
    if (_replaceInfo)
    {
        // Loop
        val.dwValue = sound.get(champ_dwEndLoop);
        if (val.dwValue != 0)
        {
            _sf2->set(id, champ_dwEndLoop, val);
            val.dwValue = sound.get(champ_dwStartLoop);
            _sf2->set(id, champ_dwStartLoop, val);
        }

        // Original pitch and correction
        if (sound.get(champ_pitchDefined) == 1)
        {
            val.bValue = (quint8)sound.get(champ_byOriginalPitch);
            _sf2->set(id, champ_byOriginalPitch, val);
            val.cValue = (char)sound.get(champ_chPitchCorrection);
            _sf2->set(id, champ_chPitchCorrection, val);
        }
    }

    // Check that start loop and and loop are not out of range
    if (_sf2->get(id, champ_dwStartLoop).dwValue > sound.get(champ_dwLength) ||
        _sf2->get(id, champ_dwEndLoop).dwValue > sound.get(champ_dwLength))
    {
        val.dwValue = 0;
        _sf2->set(id, champ_dwStartLoop, val);
        _sf2->set(id, champ_dwEndLoop, val);
    }
}
