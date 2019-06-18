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

#include "inputfactory.h"
#include <QFileInfo>
#include "soundfontmanager.h"
#include "abstractinput.h"
#include "sf2/inputsf2.h"
#include "sf3/inputsf3.h"
#include "sfz/inputsfz.h"
#include "sfark/inputsfark.h"
#include "not_supported/inputnotsupported.h"
#include "empty/inputempty.h"

AbstractInput * InputFactory::getInput(QString fileName)
{
    AbstractInput * input = nullptr;

    if (fileName.isEmpty())
    {
        // New soundfont from scratch
        input = new InputEmpty();
    }
    else
    {
        QFileInfo fileInfo(fileName);
        QString extension = fileInfo.suffix().toLower();
        if (extension == "sf2")
        {
            // Format sf2
            input = new InputSf2();
        }
        else if (extension == "sf3")
        {
            // Format sf3
            input = new InputSf3();
        }
        else if (extension == "sfz")
        {
            // Format sfz
            input = new InputSfz();
        }
        else if (extension == "sfark")
        {
            // Format sfArk
            input = new InputSfArk();
        }
        else
        {
            // Input "not supported format"
            input = new InputNotSupported();
        }
    }
    input->initialize(fileName, SoundfontManager::getInstance());

    return input;
}

bool InputFactory::isSuffixSupported(QString suffix)
{
    suffix = suffix.toLower();
    return suffix == "sf2" || suffix == "sf3" || suffix == "sfz" || suffix == "sfark";
}
