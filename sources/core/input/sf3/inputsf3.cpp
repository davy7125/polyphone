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

#include "inputsf3.h"
#include "soundfontmanager.h"
#include "sfont.h"
#include "inputfactory.h"

InputSf3::InputSf3() : AbstractInput() {}

void InputSf3::processInternal(QString fileName, SoundfontManager * sm, bool &success, QString &error, int &sf2Index, QString &tempFilePath)
{
    Q_UNUSED(sm)
    success = false;

    // Name of the temporary file
    tempFilePath = QDir::tempPath() + "/" + QFileInfo(fileName).completeBaseName() + "_tmp";
    if (QFile(tempFilePath + ".sf2").exists())
    {
        int index = 1;
        while (QFile(tempFilePath + "-" + QString::number(index) + ".sf2").exists())
            index++;
        tempFilePath = tempFilePath + "-" + QString::number(index);
    }
    tempFilePath += ".sf2";

    // First convert to sf2
    SfTools::SoundFont sf(fileName);
    if (sf.read())
    {
        QFile fo(tempFilePath);
        if (fo.open(QIODevice::WriteOnly))
        {
            if (sf.uncompress(&fo))
            {
                error = "";
                success = true;

                // Then load the sf2
                AbstractInput * sf2Input = InputFactory::getInput(tempFilePath);
                sf2Input->process(false);
                if (sf2Input->isSuccess())
                {
                    success = true;
                    sf2Index = sf2Input->getSf2Index();
                }
                else
                    error = sf2Input->getError();
                delete sf2Input;
            }
            else
                error = trUtf8("Error during the sf3 => sf2 conversion");

            fo.close();
        }
        else
            error = trUtf8("Cannot create file \"%1\"").arg(tempFilePath);
    }
    else
        error = trUtf8("Cannot read file \"%1\"").arg(fileName);
}
