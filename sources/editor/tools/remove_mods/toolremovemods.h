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

#ifndef TOOLREMOVEMODS_H
#define TOOLREMOVEMODS_H

#include "abstracttooliterating.h"

class ToolRemoveMods: public AbstractToolIterating
{
    Q_OBJECT

public:
    ToolRemoveMods();

    /// Icon, label and category displayed to the user to describe the tool
    QString getIconName() const override
    {
        return ":/tool/remove_mods.svg";
    }

    QString getCategory() const override
    {
        return trUtf8("Modulators");
    }

    /// Internal identifier
    QString getIdentifier() const override
    {
        QString identifier;
        switch (_deletionType)
        {
        case DeletionGlobal:
            identifier = "sf2:removeMods";
            break;
        case DeletionForInstrument:
            identifier = "inst:removeMods";
            break;
        case DeletionForPreset:
            identifier = "prst:removeMods";
            break;
        }
        return identifier;
    }

    /// Method executed before the iterating process
    void beforeProcess(IdList ids) override;

    /// Process an element
    void process(SoundfontManager * sm, EltID id, AbstractToolParameters * parameters) override;

    /// Get a confirmation message after the tool is run
    QString getConfirmation() override;

    /// Get the warning to display after the tool is run
    QString getWarning() override;

protected:
    QString getLabelInternal() const override
    {
        return trUtf8("Remove modulators");
    }

private:
    enum DeletionType
    {
        DeletionGlobal,
        DeletionForInstrument,
        DeletionForPreset
    };

    void clearModInst(SoundfontManager *sm, EltID idInst);
    void clearModPrst(SoundfontManager *sm, EltID idPrst);
    void clearMod(SoundfontManager *sm, EltID idMod);

    DeletionType _deletionType;
    int _count;
};

#endif // TOOLREMOVEMODS_H
