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

#ifndef SF2PDTAPART_PHDR_H
#define SF2PDTAPART_PHDR_H

#include "basetypes.h"
#include <QString>

class Sf2PdtaPart_phdr
{
public:
    Sf2PdtaPart_phdr();

    bool _isValid;

    QString _name;
    quint16Reversed _preset;
    quint16Reversed _bank;
    quint16Reversed _pBagIndex;
    quint32Reversed _library;
    quint32Reversed _genre;
    quint32Reversed _morphology;
};

// Extension methods for QDataStream to serialize / deserialize
QDataStream & operator >> (QDataStream &in, Sf2PdtaPart_phdr &phdr);
//QDataStream & operator << (QDataStream &out, const Sf2PdtaPart_phdr &phdr);

#endif // SF2PDTAPART_PHDR_H
