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

#ifndef TOOLCHANGEATTENUATION_H
#define TOOLCHANGEATTENUATION_H

#include "abstracttooliterating.h"
#include <QObject>

class ToolChangeAttenuation: public AbstractToolIterating
{
    Q_OBJECT

public:
    ToolChangeAttenuation();

    /// Icon, label and category displayed to the user to describe the tool
    QString getIconName() const override
    {
        return ":/tool/change_attenuation.svg";
    }

    QString getCategory() const override
    {
        return trUtf8("Fast editing");
    }

    /// Internal identifier
    QString getIdentifier() const override
    {
        return _isInst ? "inst:changeAttenuation" : "prst:changeAttenuation";
    }

    /// Method executed before the iterating process
    void beforeProcess(IdList ids) override;

    /// Process an element
    void process(SoundfontManager * sm, EltID id, AbstractToolParameters * parameters) override;

protected:
    QString getLabelInternal() const override
    {
        return trUtf8("Change attenuations");
    }

private:
    int limitOffset(int offset);
    bool _isInst;
};

#endif // TOOLCHANGEATTENUATION_H
