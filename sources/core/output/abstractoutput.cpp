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
    _sm(nullptr),
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
