/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2020 Davy Triponney                                **
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

#ifndef GRANDORGUERANK_H
#define GRANDORGUERANK_H

#include <QMap>
#include "basetypes.h"
class GrandOrguePipe;
class SoundfontManager;
class GrandOrgueDataThrough;

class GrandOrgueRank
{
public:
    GrandOrgueRank(QString rootDir, GrandOrgueDataThrough * godt, int id);
    ~GrandOrgueRank();

    void readData(QString key, QString value);
    void preProcess();
    EltID process(SoundfontManager * sm, int sf2Index, int indexOfFirstSample, int keyOfFirstSample);
    bool isValid();

private:
    void mergeAmplitude(int amplitude);
    void disableModulators(SoundfontManager * sm, EltID idInst);

    QString _rootDir;
    GrandOrgueDataThrough * _godt;
    int _id;

    QMap<int, GrandOrguePipe *> _pipes;
    QMap<QString, QString> _properties;
    double _gain;
    int _tuning;
};

#endif // GRANDORGUERANK_H
