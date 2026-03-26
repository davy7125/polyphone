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

#include "outputcsv.h"
#include "soundfontmanager.h"
#include "csv/csvfilesamples.h"
#include "csv/csvfileinstprst.h"
#include "utils.h"

OutputCsv::OutputCsv() : AbstractOutput() {}

void OutputCsv::processInternal(QString fileName, SoundfontManager * sm, bool &success, QString &error, int sf2Index, QMap<QString, QVariant> & options)
{
    bool rawValues = false;
    if (options.contains("raw_values"))
        rawValues = options["raw_values"].toBool();

    // Create the base directory (possibly remove the extension .csv first)
    if (fileName.endsWith(".csv"))
        fileName = QFileInfo(fileName).absolutePath() + "/" + QFileInfo(fileName).completeBaseName();
    if (QDir(fileName).exists())
    {
        int i = 1;
        while (QDir(fileName + "-" + QString::number(i)).exists())
            i++;
        fileName += "-" + QString::number(i);
    }
    if (!QDir().mkdir(fileName))
    {
        success = false;
        error = tr("cannot create directory \"%1\"").arg(fileName);
        return;
    }

    // Sample data
    CsvFileSamples csvSamples(fileName + "/samples.csv");
    if (!csvSamples.writeSamples(sm, EltID(elementSmpl, sf2Index)))
    {
        error = csvSamples.getError();
        success = false;
        return;
    }

    // Instrument data
    EltID instId(elementInst, sf2Index);
    foreach (int instNumber, sm->getSiblings(instId))
    {
        instId.indexElt = instNumber;

        // Name of the file
        QString name = Utils::removeForbiddenFilePathCharacters(sm->getQstr(instId, champ_name));

        CsvFileInstPrst csvInstrument(
            fileName + QString("/instrument %1 %2.csv").arg(instNumber, 3, 10, QChar('0')).arg(name),
            false, rawValues);
        if (!csvInstrument.write(sm, instId))
        {
            error = csvSamples.getError();
            success = false;
            return;
        }
    }

    // Preset data
    EltID prstId(elementPrst, sf2Index);
    foreach (int prstNumber, sm->getSiblings(prstId))
    {
        prstId.indexElt = prstNumber;

        // Name of the file
        QString name = Utils::removeForbiddenFilePathCharacters(sm->getQstr(prstId, champ_name));

        // Bank / preset
        quint16 bank = sm->get(prstId, champ_wBank).wValue;
        quint16 preset = sm->get(prstId, champ_wPreset).wValue;

        CsvFileInstPrst csvPreset(
            fileName + QString("/preset %1_%2 %3.csv").arg((long)bank, 3, 10, QChar('0')).arg((long)preset, 3, 10, QChar('0')).arg(name),
            true, rawValues);
        if (!csvPreset.write(sm, prstId))
        {
            error = csvSamples.getError();
            success = false;
            return;
        }
    }

    // Nothing special
    success = true;
    error = "";
}
