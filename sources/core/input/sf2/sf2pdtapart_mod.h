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

#ifndef SF2PDTAPART_MOD_H
#define SF2PDTAPART_MOD_H

#include "basetypes.h"

class Sf2PdtaPart_mod
{
public:
    Sf2PdtaPart_mod();

    bool _isValid;

    SFModulator _sfModSrcOper;
    quint16Reversed _sfModDestOper;
    qint16Reversed _modAmount;
    SFModulator _sfModAmtSrcOper;
    quint16Reversed _sfModTransOper;
};

// Extension methods for QDataStream to serialize / deserialize
QDataStream & operator >> (QDataStream &in, Sf2PdtaPart_mod &mod);
//QDataStream & operator << (QDataStream &out, const Sf2PdtaPart_mod &mod);

#endif // SF2PDTAPART_MOD_H
