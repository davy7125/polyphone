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

#ifndef BASETYPES_H
#define BASETYPES_H

#include "idlist.h"
#include "eltid.h"
#include "complex.h"
#include "attribute.h"
#include "modulatordata.h"
#include "serializabletypes.h"

#define DB_SF2_TO_SFZ 0.397

// In synth.h
#define MAX_NUMBER_OF_VOICES_TO_ADD 128

// In soundengine.h
#define MAX_NUMBER_OF_VOICES 256

// In voice.h
#define INITIAL_ARRAY_LENGTH 1024

// In modulatorgroup.h
#define MAX_NUMBER_OF_PARAMETER_MODULATORS 64

#endif // BASETYPES_H
