#include "abstractoutput.h"
#include <QFuture>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtConcurrent/QtConcurrent>
#else
#include <QFutureWatcher>
#include <QtConcurrentRun>
#endif

AbstractOutput::AbstractOutput() : QObject(),
    _futureWatcher(new QFutureWatcher<void>()),
    _sm(NULL),
    _fileName(""),
    _isSuccess(false),
    _error(trUtf8("not processed yet")),
    _sf2Index(-1)
{
    connect(_futureWatcher, SIGNAL(finished()), this, SIGNAL(finished()), Qt::QueuedConnection);
}

AbstractOutput::~AbstractOutput()
{
    delete _futureWatcher;
}

void AbstractOutput::initialize(QString fileName, SoundfontManager * sm)
{
    _fileName = fileName;
    _sm = sm;
}

void AbstractOutput::setOption(QString optionName, QVariant value)
{
    _options[optionName] = value;
}

void AbstractOutput::process(int sf2Index, bool async)
{
    _sf2Index = sf2Index;

    if (async)
    {
        QFuture<void> future = QtConcurrent::run(this, &AbstractOutput::processAsync);
        _futureWatcher->setFuture(future);
    }
    else
        processAsync();
}

void AbstractOutput::processAsync()
{
    // Parse the file
    this->processInternal(_fileName, _sm, _isSuccess, _error, _sf2Index, _options);
}
