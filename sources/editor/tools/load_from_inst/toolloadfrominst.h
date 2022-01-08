/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2020 Davy Triponney                                **
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

#ifndef TOOLLOADFROMINST_H
#define TOOLLOADFROMINST_H

#include "abstracttoolonestep.h"
#include <QObject>

class ToolLoadFromInst: public AbstractToolOneStep
{
    Q_OBJECT

public:
    ToolLoadFromInst();

    /// Icon, label and category displayed to the user to describe the tool
    QString getIconName() const override
    {
        return ":/tool/load_from_inst.svg";
    }

    QString getCategory() const override
    {
        return tr("Fast editing");
    }

    /// Internal identifier
    QString getIdentifier() const override
    {
        return "smpl:loadFromInst";
    }

    /// Process an element
    void process(SoundfontManager * sm, IdList ids, AbstractToolParameters * parameters) override;

protected:
    QString getLabelInternal() const override
    {
        return tr("Retrieve root key from instruments");
    }

    /// Return true if the tool can be used on the specified ids
    bool isCompatible(IdList ids) override;

    /// Get a confirmation message after the tool is run
    QString getConfirmation() override;

    /// Get the warning to display after the tool is run
    QString getWarning() override;

private:
    QList<int> _smplIdsWithNoValues, _smplIdsWithOneValue, _smplIdsWithSeveralValues;
};

#endif // TOOLLOADFROMINST_H
