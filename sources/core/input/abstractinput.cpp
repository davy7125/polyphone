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
    _sm(nullptr),
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
        EltID idSf2(elementSf2);
        foreach (int i, _sm->getSiblings(idSf2))
        {
            if (_sm->getQstr(EltID(elementSf2, i), champ_filenameInitial) == _fileName)
            {
                _isSuccess = false;
                _error = trUtf8("This file is already open.");
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
