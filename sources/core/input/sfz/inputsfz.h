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

#ifndef INPUTSFZ_H
#define INPUTSFZ_H

#include "abstractinput.h"
#include "sfzparametergroupassembly.h"
class SoundfontManager;

class InputSfz : public AbstractInput
{
    Q_OBJECT
    
public:
    InputSfz();

protected slots:
    void processInternal(QString fileName, SoundfontManager * sm, bool &success, QString &error, int &sf2Index, QString &tempFilePath) override;

private:
    enum Bloc
    {
        BLOC_UNKNOWN,
        BLOC_CONTROL,
        BLOC_GLOBAL,
        BLOC_GROUP,
        BLOC_REGION
    };

    QList<SfzParameterGroupAssembly> _listeEnsembles;
    Bloc _currentBloc;
    SfzParameterGroup _globalZone;
    QStringList _openFilePaths;
    QString _rootDir;
    QMap<QString, QString> _replacements;

    void parseFile(QString filename, bool &success, QString &error);
    QString applyReplacements(QString opcodeValue);
    QString getFilePathFromInclude(QString str);
    void changeBloc(QString bloc);
    void addOpcode(QString opcode, QString value);

    void createSf2(int &sf2Index, QString filename, bool isChannel10);
    QString getInstrumentName(QString filePath, int &numBank, int &numPreset);
};

#endif // INPUTSFZ_H
