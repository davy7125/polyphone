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

#include "toolfrequencypeaks.h"
#include "toolfrequencypeaks_gui.h"
#include "soundfontmanager.h"
#include "contextmanager.h"

ToolFrequencyPeaks::ToolFrequencyPeaks() : AbstractToolOneStep(new ToolFrequencyPeaks_parameters(), new ToolFrequencyPeaks_gui())
{

}

bool ToolFrequencyPeaks::isCompatible(IdList ids)
{
    return !ids.getSelectedIds(elementSmpl).empty();
}

void ToolFrequencyPeaks::process(SoundfontManager * sm, IdList ids, AbstractToolParameters * parameters)
{
    Q_UNUSED(sm)
    Q_UNUSED(ids)
    ToolFrequencyPeaks_parameters * params = (ToolFrequencyPeaks_parameters *) parameters;

    // Création fichier csv
    QFile file(params->getFilePath());
    if (!file.open(QIODevice::WriteOnly))
        return;

    // Header
    QString sep = ",";
    QTextStream stream(&file);
    stream << "\"" << trUtf8("Sample") << "\"" << sep << "\"" << trUtf8("Peak") << "\"" << sep << "\""
           << trUtf8("Intensity") << "\"" << sep << "\"" << trUtf8("Frequency") << "\"" << sep << "\""
           << trUtf8("Key") << "\"" << sep << "\"" << trUtf8("Correction") << "\"";

    // Data
    foreach (SampleFrequencyInfo sfi, params->getSampleFrequencies())
    {
        QString sampleName = sfi.name.replace(QRegExp(QString::fromUtf8("[`~*|:<>«»?/{}\"\\\\]")), "_");

        int number = 1;
        foreach (FrequencyInfo fi, sfi.frequencies)
        {
            stream << endl;
            stream << "\"" << sampleName << "\"" << sep;
            stream << number << sep;
            stream << QString::number(fi.factor) << sep;
            stream << QString::number(fi.frequency) << sep;
            stream << ContextManager::keyName()->getKeyName(fi.key) << sep;
            stream << QString::number(fi.correction);
            number++;
        }
    }
    file.close();
}

QString ToolFrequencyPeaks::getConfirmation()
{
    return trUtf8("Success");
}
