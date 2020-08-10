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

#include "outputfactory.h"
#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>
#include <QApplication>
#include "soundfontmanager.h"
#include "contextmanager.h"
#include "abstractoutput.h"
#include "sf2/outputsf2.h"
#include "sfz/outputsfz.h"
#include "not_supported/outputnotsupported.h"
#include "empty/outputdummy.h"

AbstractOutput * OutputFactory::getOutput(QString fileName)
{
    AbstractOutput * output = nullptr;

    if (fileName.isEmpty())
    {
        // Do nothing
        output = new OutputDummy();
    }
    else
    {
        QFileInfo fileInfo(fileName);
        QString extension = fileInfo.suffix().toLower();
        if (extension == "sf2")
        {
            // Format sf2
            output = new OutputSf2();
        }
        else if (extension == "sfz")
        {
            // Format sfz
            output = new OutputSfz();
        }
        else
        {
            // Input "not supported format"
            output = new OutputNotSupported();
        }
    }
    output->initialize(fileName, SoundfontManager::getInstance());

    return output;
}

bool OutputFactory::save(int indexSf2, bool saveAs)
{
    // Check that the soundfont is valid
    SoundfontManager * sm = SoundfontManager::getInstance();
    EltID id(elementSf2, indexSf2);
    if (indexSf2 == -1 || !sm->isValid(id))
        return false;

    // Don't go further if the file is already saved
    if (!sm->isEdited(id.indexSf2) && !saveAs)
        return false;

    // Path of the file for saving the soundfont
    QString savePath;
    QString filePathInitial = sm->getQstr(id, champ_filenameInitial);
    QString filePathForData = sm->getQstr(id, champ_filenameForData);
    if (saveAs || !filePathInitial.toLower().endsWith(".sf2") ||
            filePathInitial != filePathForData || filePathInitial.isEmpty())
    {
        // Default path for selecting the destination
        QString defaultPath;
        if (filePathInitial.isEmpty())
        {
            // A new file is to be saved, the path is based on the internal name and the recent files
            QString currentName = sm->getQstr(id, champ_name);
            if (currentName.isEmpty())
                currentName = QObject::tr("untitled");
            defaultPath = ContextManager::recentFile()->getLastDirectory(RecentFileManager::FILE_TYPE_SOUNDFONT) + "/" + currentName + ".sf2";
        }
        else if (filePathInitial != filePathForData || !filePathInitial.toLower().endsWith(".sf2"))
        {
            // The soundfont to be saved was imported => the path is based on the initial file with another extension
            QFileInfo fi(filePathInitial);
            defaultPath = fi.absolutePath() + "/" + fi.completeBaseName() + ".sf2";
        }
        else
            defaultPath = filePathInitial;

        // Dialog for choosing a destination
        savePath = QFileDialog::getSaveFileName(QApplication::activeWindow(), QObject::tr("Save a soundfont"),
                                                defaultPath, QObject::tr("Sf2 files") + " (*.sf2)");
        if (savePath.isNull())
            return false;
    }
    else
        savePath = filePathInitial;

    AbstractOutput * output = OutputFactory::getOutput(savePath);
    output->process(id.indexSf2, false);

    bool success = output->isSuccess();
    if (success)
    {
        // Possibly delete a temporary file
        if (filePathInitial != filePathForData && !filePathForData.isEmpty())
            QFile::remove(filePathForData);

        // New recent file
        ContextManager::recentFile()->addRecentFile(RecentFileManager::FILE_TYPE_SOUNDFONT, savePath);
    }
    else
        QMessageBox::warning(QApplication::activeWindow(), QObject::tr("Warning"), output->getError());
    delete output;

    return success;
}
