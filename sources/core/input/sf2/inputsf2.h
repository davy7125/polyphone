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

#ifndef INPUTSF2_H
#define INPUTSF2_H

#include "abstractinput.h"
class SoundfontManager;
class Sf2Header;
class Sf2SdtaPart;
class Sf2PdtaPart;

class InputSf2 : public AbstractInput
{
    Q_OBJECT
    
public:
    InputSf2();

protected slots:
    void processInternal(QString fileName, SoundfontManager * sm, bool &success, QString &error, int &sf2Index, QString &tempFilePath) override;

private:
    void parse(QDataStream &stream, bool &success, QString &error, int &sf2Index);
    void fillSf2(Sf2Header &header, Sf2SdtaPart &sdtaPart, Sf2PdtaPart &pdtaPart, bool &success, QString &error, int &sf2Index);

    SoundfontManager * _sm;
    QString _filename;
};

#endif // INPUTSF2_H
