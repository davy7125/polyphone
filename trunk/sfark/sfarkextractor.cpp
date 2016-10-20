/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2016 Davy Triponney                                **
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
**  Website/Contact: http://polyphone-soundfonts.com                      **
**             Date: 01.01.2013                                           **
***************************************************************************/

#include "sfarkextractor.h"
#include "sfarkextractor1.h"
#include "sfarkextractor2.h"

#include <QProgressDialog>
#include <QFutureWatcher>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtConcurrent/QtConcurrentRun>
#else
#include <QtConcurrentRun>
#endif

SfArkExtractor::SfArkExtractor(const char * fileName, QWidget *parent) : AbstractExtractor(),
    _extractor(NULL),
    _parent(parent)
{
    bool isVersion1;
    SfArkExtractor1 * extractorTmp = new SfArkExtractor1(fileName, isVersion1);
    delete extractorTmp;

    if (isVersion1)
        _extractor = new SfArkExtractor1(fileName);
    else
        _extractor = new SfArkExtractor2(fileName);
}

SfArkExtractor::~SfArkExtractor()
{
    delete _extractor;
}

void SfArkExtractor::extract()
{
    if (_parent != NULL)
    {
        QProgressDialog dialog(_parent);
        dialog.setLabelText(QObject::trUtf8("Extraction en cours..."));
        dialog.setCancelButtonText(QString());
        dialog.setRange(0, 0);
        dialog.setWindowFlags(dialog.windowFlags() & ~Qt::WindowContextHelpButtonHint);

        QFutureWatcher<void> watcher;
        QObject::connect(&watcher, SIGNAL(finished()), &dialog, SLOT(reset()));
        watcher.setFuture(QtConcurrent::run(this, &SfArkExtractor::extract2));

        dialog.exec();
    }
    else
        extract2();
}

bool SfArkExtractor::getData(char *&data, qint32 &size)
{
    return _extractor->getData(data, size);
}

void SfArkExtractor::extract2()
{
    _extractor->extract();
}
