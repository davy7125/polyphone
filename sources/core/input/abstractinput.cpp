#include "abstractinput.h"
#include "basetypes.h"
#include "soundfontmanager.h"
#include <QFuture>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtConcurrent/QtConcurrent>
#else
#include <QFutureWatcher>
#include <QtConcurrentRun>
#endif

AbstractInput::AbstractInput() : QObject(),
    _futureWatcher(new QFutureWatcher<void>()),
    _sm(NULL),
    _fileName(""),
    _isSuccess(false),
    _error(trUtf8("pas encore traité")),
    _sf2Index(-1)
{
    connect(_futureWatcher, SIGNAL(finished()), this, SIGNAL(finished()), Qt::QueuedConnection);
}

AbstractInput::~AbstractInput()
{
    delete _futureWatcher;
}

void AbstractInput::initialize(QString fileName, SoundfontManager * sm)
{
    _fileName = fileName;
    _sm = sm;
}

void AbstractInput::process(bool async)
{
    if (async)
    {
        QFuture<void> future = QtConcurrent::run(this, &AbstractInput::processAsync);
        _futureWatcher->setFuture(future);
    }
    else
        processAsync();
}

void AbstractInput::processAsync()
{
    // Check that the file is not already open
    if (!_fileName.isEmpty())
    {
        foreach (int i, _sm->getSiblings(EltID(elementSf2)))
        {
            if (_sm->getQstr(EltID(elementSf2, i), champ_filenameInitial) == _fileName)
            {
                _isSuccess = false;
                _error = trUtf8("Le fichier est déjà ouvert");
                return;
            }
        }
    }

    // Parse the file
    QString tempFilePath = "";
    this->processInternal(_fileName, _sm, _isSuccess, _error, _sf2Index, tempFilePath);

    // Keep a track of the files
    if (_isSuccess)
    {
        EltID id(elementSf2, _sf2Index);
        _sm->set(id, champ_filenameInitial, _fileName);
        _sm->set(id, champ_filenameForData, tempFilePath.isEmpty() ? _fileName : tempFilePath);
    }
    else if (!tempFilePath.isEmpty())
        QFile::remove(tempFilePath);

    // The operation are not stored in the action manager
    _sm->clearNewEditing();
}
