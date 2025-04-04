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

#ifndef INPUTSFZ_H
#define INPUTSFZ_H

#include "abstractinput.h"
#include <QObject>

class InputSfz : public AbstractInput
{
public:
    /// Description of the file type to open
    QString getInputDescription() override { return QObject::tr("Sfz files"); }

    /// Extension of the file type to open
    QStringList getInputExtensions() override { return QStringList() << "sfz"; }

    /// Return a parser
    AbstractInputParser * getParser(QString filename) override;
};

#endif // INPUTSFZ_H
