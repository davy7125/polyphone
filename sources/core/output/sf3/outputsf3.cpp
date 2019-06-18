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

#include "outputsf3.h"
#include "sfont.h"
#include "outputfactory.h"
#include <QFile>
#include <QVariant>

OutputSf3::OutputSf3() : AbstractOutput() {}

void OutputSf3::processInternal(QString fileName, SoundfontManager * sm, bool &success, QString &error, int sf2Index, QMap<QString, QVariant> & options)
{
    Q_UNUSED(sm)

    // First save in the sf2 format
    QString fileNameSf2 = fileName.left(fileName.length() - 4) + "_tmp";
    if (QFile(fileNameSf2 + ".sf2").exists())
    {
        int index = 1;
        while (QFile(fileNameSf2 + "-" + QString::number(index) + ".sf2").exists())
            index++;
        fileNameSf2 = fileNameSf2 + "-" + QString::number(index);
    }
    fileNameSf2 += ".sf2";

    AbstractOutput * outputSf2 = OutputFactory::getOutput(fileNameSf2);
    outputSf2->process(sf2Index, false);
    error = outputSf2->getError();
    success = outputSf2->isSuccess();
    delete outputSf2;
    if (!success)
        return;

    // Then create an sf3 file
    int quality = options.contains("quality") ? options["quality"].toInt() : 1;
    double qualityValue = 1.0;
    switch (quality)
    {
    case 0: qualityValue = 0.2; break;
    case 1: qualityValue = 0.6; break;
    case 2: qualityValue = 1.0; break;
    }

    SfTools::SoundFont sf(fileNameSf2);
    if (sf.read())
    {
        QFile fo(fileName);
        if (fo.open(QIODevice::WriteOnly))
        {
            if (sf.compress(&fo, qualityValue, -1.0))
            {
                error = "";
                success = true;
            }
            else
            {
                error = trUtf8("Error during the sf3 conversion");
                success = false;
            }
            fo.close();
        }
        else
        {
            error = trUtf8("Cannot create file \"%1\"").arg(fileName);
            success = false;
        }
    }
    else
    {
        error = trUtf8("Cannot read file \"%1\"").arg(fileNameSf2);
        success = false;
    }

    // Delete the sf2 temporary file and the sf2
    QFile::remove(fileNameSf2);
}
