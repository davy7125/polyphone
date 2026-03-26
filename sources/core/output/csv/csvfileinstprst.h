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

#ifndef CSVFILEINSTPRST_H
#define CSVFILEINSTPRST_H

#include "csv/abstractcsvfile.h"
#include "basetypes.h"
class SoundfontManager;

class CsvFileInstPrst: public AbstractCsvFile
{
public:
    CsvFileInstPrst(QString filePath, bool isPrst, bool rawValues);
    bool write(SoundfontManager * sm, EltID idElt);

protected:
    QList<QString> getHeaders() override;

private:
    QString getRValue(AttributeType type);
    QString getShValue(AttributeType type);
    QString getWValue(AttributeType type);
    AttributeValue * getAttributeValue(AttributeType type);

    bool _isPrst;
    bool _rawValues;

    AttributeValue _defaultValue;
    AttributeValue * _globalAttributes;
    AttributeValue * _attributes;
    bool * _globalAttributesSet;
    bool * _attributesSet;
};

#endif // CSVFILEINSTPRST_H
