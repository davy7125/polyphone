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

#ifndef TOOLCHANGEATTENUATION_H
#define TOOLCHANGEATTENUATION_H

#include "abstracttooliterating.h"
#include <QObject>
#include <QAtomicInt>

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
        return tr("Fast editing");
    }

    /// Internal identification
    QString getToolName() const override
    {
        return "volume";
    }

    /// Method executed before the iterating process
    void beforeProcess(IdList ids) override;

    /// Process an element
    void process(SoundfontManager * sm, EltID id, AbstractToolParameters * parameters) override;

    /// Number of attenuations limited to their range (0 to 57.6 dB in an instrument, -57.6 to 57.6 dB in a preset)
    /// during the last run
    int getClampedValueCount() const { return _clampedValues.loadRelaxed(); }

protected:
    QString getLabelInternal() const override
    {
        return tr("Change attenuations");
    }

private:
    int limitOffset(int offset);
    bool _isInst;
    QAtomicInt _clampedValues;
};

#endif // TOOLCHANGEATTENUATION_H
