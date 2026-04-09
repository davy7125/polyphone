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
#include <QVectorIterator>
#include "basetypes.h"
class AbstractInputParser;
class Smpl;
class InstPrst;

class DirectoryFileData
{
public:
    struct DetailsData
    {
        QStringList names;
        QStringList details;
        QList<int> values;
    };

    enum Status {
        NOT_INITIALIZED,
        NOT_READABLE,
        NOT_OPENABLE,
        NOT_SCANNABLE,
        CANNOT_SCAN,
        OK
    };

    DirectoryFileData(const QFileInfo &fileInfo);

    QString getPath() const { return _path; }
    QString getFileName() const { return QFileInfo(_path).fileName(); }
    quint64 getFileSize() const { return _fileSize; }
    QDateTime getLastModified() const { return _lastModified; }
    Status getStatus() const { return _status; }

    int getSampleCount() const { return _samples.count(); }
    DetailsData getSampleDetails() const;

    int getInstrumentCount() const { return _instruments.count(); }
    DetailsData getInstrumentDetails() const;

    int getPresetCount() const { return _presets.count(); }
    DetailsData getPresetDetails() const;

private:
    class DirectorySampleData
    {
    public:
        QString getDetail();

        int id;
        QString name;
        QString nameSort;
        SFSampleLink sampleType = linkInvalid;
        int samplingRateHz = 0;
        int totalDurationMilliSec = 0;
        int loopDurationMilliSec = 0;
    };

    class DirectoryInstrumentPresetData
    {
    public:
        QString getDetail();

        int id;
        QString name;
        QString nameSort;
        int numDivisions = 0;
        int numDistinctKeyRanges = 0;
        int numDistinctVelocityRanges = 0;
        int numParameters = 0;
        int numModulators = 0;
    };

    bool scan(AbstractInputParser * parser);
    static void scanSmpl(QVectorIterator<Smpl*> &i, QList<DirectorySampleData> &list);
    static void scanInstPrst(QVectorIterator<InstPrst*> &i, QList<DirectoryInstrumentPresetData> &list, bool isPrst);

    QString _path;
    quint64 _fileSize;
    QDateTime _lastModified;
    Status _status;
    QList<DirectorySampleData> _samples;
    QList<DirectoryInstrumentPresetData> _instruments;
    QList<DirectoryInstrumentPresetData> _presets;
};

Q_DECLARE_METATYPE(const DirectoryFileData *)

#endif // DIRECTORYFILEDATA_H
