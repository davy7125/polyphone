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

#ifndef TOOLUNLINKSAMPLE_H
#define TOOLUNLINKSAMPLE_H

#include "abstracttooliterating.h"
#include <QObject>

class ToolUnlinkSample: public AbstractToolIterating
{
    Q_OBJECT

public:
    ToolUnlinkSample() : AbstractToolIterating(elementSmpl)
    {
        _async = false;
    }

    /// Icon, label and category displayed to the user to describe the tool
    QString getIconName() const override
    {
        return ":/tool/unlink.svg";
    }

    QString getCategory() const override
    {
        return tr("Stereo samples");
    }

    /// Internal identification
    QString getToolName() const override
    {
        return "unlink";
    }

    /// Process an element
    void process(SoundfontManager * sm, EltID id, AbstractToolParameters * parameters) override;

protected:
    QString getLabelInternal() const override
    {
        return tr("Unlink");
    }

private:
    void unlink(SoundfontManager * sm, EltID idSample);
};

#endif // TOOLUNLINKSAMPLE_H
