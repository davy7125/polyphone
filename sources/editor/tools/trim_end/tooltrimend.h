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

#ifndef TOOLTRIMEND_H
#define TOOLTRIMEND_H

#include "abstracttooliterating.h"
#include <QObject>

class ToolTrimEnd: public AbstractToolIterating
{
    Q_OBJECT

public:
    ToolTrimEnd() : AbstractToolIterating(elementSmpl) {}

    /// Icon, label and category displayed to the user to describe the tool
    QString getIconName() const override
    {
        return ":/tool/trim_loop.svg";
    }

    QString getCategory() const override
    {
        return tr("Loop / playback");
    }

    /// Internal identification
    QString getToolName() const override
    {
        return "trimEnd";
    }

    /// Process an element
    void process(SoundfontManager * sm, EltID id, AbstractToolParameters * parameters) override;

    /// Trim a sample
    static void trim(EltID id);

protected:
    QString getLabelInternal() const override
    {
        return tr("Trim to end of loop");
    }
};

#endif // TOOLTRIMEND_H
