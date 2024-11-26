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

    /// Internal identification
    QString getToolName() const override
    {
        return "loadFromInst";
    }

    /// Process an element
    void process(SoundfontManager * sm, IdList ids, AbstractToolParameters * parameters) override;

protected:
    QString getLabelInternal() const override
    {
        return tr("Load a parameter from the instruments");
    }

    /// Return true if the tool can be used on the specified ids
    bool isCompatible(IdList ids) override;

    /// Get a confirmation message after the tool is run
    QString getConfirmation() override;

    /// Get the warning to display after the tool is run
    QString getWarning() override;

private:
    class ScanResult
    {
    public:
        ScanResult(EltID id) :
            _id(id),
            _value1(0),
            _value2(0),
            _value3(0),
            _value4(0),
            _isSet(false),
            _cannotBeSet(false)
        {}
        ScanResult(EltID id, ScanResult &other) :
            _id(id),
            _value1(other._value1),
            _value2(other._value2),
            _value3(other._value3),
            _value4(other._value4),
            _isSet(other._isSet),
            _cannotBeSet(other._cannotBeSet)
        {}

        EltID _id;
        int _value1, _value2, _value3, _value4;
        bool _isSet, _cannotBeSet;
    };

    void readParameter(SoundfontManager * sm, int parameter, ScanResult &scanResult);
    bool processParameter(SoundfontManager *sm, int parameter, EltID id, QList<ScanResult> values);

    int _failCount;
};

#endif // TOOLLOADFROMINST_H
