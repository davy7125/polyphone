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

#ifndef TOOLFREQUENCYPEAKS_PARAMETERS_H
#define TOOLFREQUENCYPEAKS_PARAMETERS_H

#include "abstracttoolparameters.h"
#include <QList>
#include <QString>
#include <QMetaType>

class FrequencyInfo
{
public:
    double frequency;
    double factor;
    int key;
    int correction;
};

Q_DECLARE_METATYPE(FrequencyInfo)

class SampleFrequencyInfo
{
public:
    QString name;
    QList<FrequencyInfo> frequencies;
};

Q_DECLARE_METATYPE(SampleFrequencyInfo)

class ToolFrequencyPeaks_parameters: public AbstractToolParameters
{
public:
    /// Load the configuration from the ini file
    void loadConfiguration() override;

    /// Save the configuration in the ini file
    void saveConfiguration() override;

    /// Transitional data
    QList<SampleFrequencyInfo> getSampleFrequencies() { return _sampleFrequencies; }
    void setSampleFrequencies(QList<SampleFrequencyInfo> sampleFrequencies) { _sampleFrequencies = sampleFrequencies; }

    QString getFilePath() { return _filePath; }
    void setFilePath(QString filePath) { _filePath = filePath; }

private:
    QList<SampleFrequencyInfo> _sampleFrequencies;
    QString _filePath;
};

#endif // TOOLFREQUENCYPEAKS_PARAMETERS_H
