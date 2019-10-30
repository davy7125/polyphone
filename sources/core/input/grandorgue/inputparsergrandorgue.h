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

#ifndef INPUTPARSERGRANDORGUE_H
#define INPUTPARSERGRANDORGUE_H

#include <QMap>
#include "abstractinputparser.h"
class GrandOrgueRank;
class GrandOrgueStop;
class GrandOrgueDataThrough;
class SoundfontManager;

class InputParserGrandOrgue : public AbstractInputParser
{
    Q_OBJECT
    
public:
    InputParserGrandOrgue();
    ~InputParserGrandOrgue() override;

protected slots:
    void processInternal(QString fileName, SoundfontManager * sm, bool &success, QString &error, int &sf2Index, QString &tempFilePath) override;

private:
    enum Section
    {
        SECTION_UNKNOWN,
        SECTION_STOP,
        SECTION_ORGAN,
        SECTION_RANK
    };

    void parseFile(QString filename, bool &success, QString &error);
    void startSection(QString sectionName);
    void processData(QString key, QString value);
    void createSf2(int &sf2Index, QString filename);
    QString getComment();

    QString _rootDir;
    Section _currentSection;
    int _currentIndex; // Of a rank or stop, -1 if unknown
    GrandOrgueDataThrough * _godt;
    QMap<int, GrandOrgueRank*> _ranks;
    QMap<int, GrandOrgueStop*> _stops;
    QMap<QString, QString> _organProperties;
};

#endif // INPUTPARSERGRANDORGUE_H
