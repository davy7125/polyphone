/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2024 Davy Triponney                                **
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
**  Website/Contact: https://www.polyphone.io                             **
**             Date: 01.01.2013                                           **
***************************************************************************/

#include "outputfactory.h"
#include <QFileInfo>
#include "soundfontmanager.h"
#include "abstractoutput.h"
#include "sf/outputsf.h"
#include "sfz/outputsfz.h"
#include "dls/outputdls.h"
#include "not_supported/outputnotsupported.h"
#include "empty/outputdummy.h"
#include "csv/outputcsv.h"

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
        if (extension == "sf2" || extension == "sf3")
        {
            // Format sf2 / sf3
            output = new OutputSf();
        }
        else if (extension == "sfz")
        {
            // Format sfz
            output = new OutputSfz();
        }
        else if (extension == "dls")
        {
            // Format dls
            output = new OutputDls();
        }
        else if (extension == "csv")
        {
            // Format csv
            output = new OutputCsv();
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
