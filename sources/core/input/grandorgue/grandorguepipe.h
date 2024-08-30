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

#ifndef GRANDORGUEPIPE_H
#define GRANDORGUEPIPE_H

#include <QMap>
#include "basetypes.h"
class GrandOrgueDataThrough;

class GrandOrguePipe
{
public:
    GrandOrguePipe(QString rootDir, GrandOrgueDataThrough * godt);

    void readData(QString key, QString value);
    bool isValid();

    // Possibly add a gain from the rank or stop that includes the pipe
    void addGain(double offset) { _gain += offset; }
    void mergeAmplitude(int amplitude);
    double gain() { return _gain; }
    QString getRelativePath() { return _relativePath; }

    void addTuning(int offset) { _tuning += offset; }

    void process(EltID parent, int key);

private:
    QList<int> getSampleIds(int sf2Id, QString relativeFilePath);
    static QString getName(QString name, int maxCharacters, int suffixNumber, QString suffix = "");
    QString getReleaseFilePath();

    QString _rootDir;
    GrandOrgueDataThrough * _godt;

    QMap<QString, QString> _properties;
    QString _relativePath;
    QString _error;
    double _gain;
    int _tuning;
};

#endif // GRANDORGUEPIPE_H
