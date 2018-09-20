#include "abstractinput.h"
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
    _error(trUtf8("not processed yet")),
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

void AbstractInput::process()
{
    QFuture<void> future = QtConcurrent::run(this, &AbstractInput::processAsync);
    _futureWatcher->setFuture(future);
}

void AbstractInput::processAsync()
{
    // Parse the file
    this->processInternal(_fileName, _sm, _isSuccess, _error, _sf2Index);
}
