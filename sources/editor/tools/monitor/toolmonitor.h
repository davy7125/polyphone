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

#ifndef TOOLMONITOR_H
#define TOOLMONITOR_H

#include "abstracttool.h"

class ToolMonitor: public AbstractTool
{
    Q_OBJECT

public:
    ToolMonitor();

    /// Icon, label and category displayed to the user to describe the tool
    QString getIconName() const override
    {
        return ":/tool/monitor.svg";
    }

    QString getCategory() const override
    {
        return trUtf8("Analyze");
    }

    /// Internal identifier
    QString getIdentifier() const override
    {
        return _isInst ? "inst:monitor" : "prst:monitor";
    }

protected:
    QString getLabelInternal() const override
    {
        return trUtf8("Display parameters");
    }

    /// Return true if the tool can be used on the specified ids
    bool isCompatible(IdList ids) override;

    /// Run the tool, emit the signal "finished" at the end
    void runInternal(SoundfontManager * sm, QWidget * parent, IdList ids, AbstractToolParameters * parameters) override;

private:
    bool _isInst;
};

#endif // TOOLMONITOR_H
