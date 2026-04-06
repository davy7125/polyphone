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

#ifndef DIRECTORYFILEDATA_H
#define DIRECTORYFILEDATA_H

#include <QDateTime>
#include <QFileInfo>
#include "basetypes.h"

struct DirectorySampleData
{
    int id;
    QString name;
    SFSampleLink sampleType = linkInvalid;
    int samplingRateHz = 0;
    int totalDurationMilliSec = 0;
    int loopDurationMilliSec = 0;
};

struct DirectoryInstrumentPresetData
{
    int id;
    QString name;
    int numDivisions = 0;
    int numDistinctKeyRanges = 0;
    int numDistinctVelocityRanges = 0;
    int numParameters = 0;
    int numModulators = 0;
};

class DirectoryFileData
{
public:
    DirectoryFileData(const QFileInfo &fileInfo);

    QString getPath() { return _path; }
    QString getFileName() const;
    quint64 getFileSize() const { return _fileSize; }
    QDateTime getLastModified() const { return _lastModified; }
    QList<DirectorySampleData> getSamples() const { return _samples; }
    QList<DirectoryInstrumentPresetData> getInstruments() const { return _instruments; }
    QList<DirectoryInstrumentPresetData> getPresets() const { return _presets; }

private:
    QString _path;
    quint64 _fileSize;
    QDateTime _lastModified;
    QList<DirectorySampleData> _samples;
    QList<DirectoryInstrumentPresetData> _instruments;
    QList<DirectoryInstrumentPresetData> _presets;
};

Q_DECLARE_METATYPE(const DirectoryFileData *)

#endif // DIRECTORYFILEDATA_H
