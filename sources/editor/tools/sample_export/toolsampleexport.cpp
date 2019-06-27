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

#include "toolsampleexport.h"
#include "soundfontmanager.h"
#include "sampleutils.h"
#include "contextmanager.h"
#include "samplewriterwav.h"
#include <qmath.h>
#include <QFileDialog>
#include <QApplication>

void ToolSampleExport::beforeProcess(IdList ids)
{
    Q_UNUSED(ids)
    _exportedSamples.clear();

    // Directory in which the samples will be exported
    _dirPath = QFileDialog::getExistingDirectory(
                QApplication::activeWindow(), trUtf8("Choose a destination folder"),
                ContextManager::recentFile()->getLastDirectory(RecentFileManager::FILE_TYPE_SAMPLE));
    if (!_dirPath.isEmpty())
    {
        _dirPath += "/";
        ContextManager::recentFile()->addRecentFile(RecentFileManager::FILE_TYPE_SAMPLE, _dirPath + "sample.wav");
    }
}

void ToolSampleExport::process(SoundfontManager * sm, EltID id, AbstractToolParameters *parameters)
{
    Q_UNUSED(parameters);

    // Path specified?
    if (_dirPath.isEmpty())
        return;

    // Sample already processed?
    bool isStereo = false;
    EltID id2 = id;
    _mutex.lock();
    if (_exportedSamples.contains(id))
    {
        _mutex.unlock();
        return;
    }
    _exportedSamples << id;

    // Stereo sample?
    if (sm->get(id, champ_sfSampleType).wValue != monoSample &&
            sm->get(id, champ_sfSampleType).wValue != RomMonoSample)
    {
        id2.indexElt = sm->get(id, champ_wSampleLink).wValue;
        _exportedSamples << id2;
        isStereo = true;
    }

    // Filename of the exported sample (in the mutex also so that filenames are all different)
    QString fileName = getFilePath(sm, id, id2, isStereo);
    _mutex.unlock();

    // Export

    SampleWriterWav writer(fileName);
    if (isStereo)
    {
        // First id must be the left sound
        if (sm->get(id, champ_sfSampleType).wValue == rightSample || sm->get(id, champ_sfSampleType).wValue == RomRightSample)
        {
            EltID idTmp = id;
            id = id2;
            id2 = idTmp;
        }

        writer.write(sm->getSound(id), sm->getSound(id2));
    }
    else
        writer.write(sm->getSound(id));
}

QString ToolSampleExport::getFilePath(SoundfontManager * sm, EltID id1, EltID id2, bool isStereo)
{
    QString fileName;
    if (isStereo)
    {
        QString fileName1 = sm->getQstr(id1, champ_name);
        QString fileName2 = sm->getQstr(id2, champ_name);
        int nb = SampleUtils::lastLettersToRemove(fileName1, fileName2);
        fileName = fileName1.left(fileName1.size() - nb).replace(QRegExp("[:<>\"/\\\\\\*\\?\\|]"), "_");
    }
    else
        fileName = sm->getQstr(id1, champ_name).replace(QRegExp("[:<>\"/\\\\\\*\\?\\|]"), "_");

    QString filePath = _dirPath + fileName;

    // Already existing?
    if (QFile::exists(filePath + ".wav"))
    {
        // Add a suffix
        int indice = 1;
        while (QFile::exists(filePath + "-" + QString::number(indice) + ".wav"))
            indice++;
        filePath += "-" + QString::number(indice);
    }

    return filePath + ".wav";
}
