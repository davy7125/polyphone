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

#ifndef TOOLLINKSAMPLE_H
#define TOOLLINKSAMPLE_H

#include "abstracttooliterating.h"
#include <QObject>

class ToolLinkSample: public AbstractToolIterating
{
    Q_OBJECT

public:
    ToolLinkSample() : AbstractToolIterating(elementSmpl)
    {
        _async = false;
    }

    /// Icon, label and category displayed to the user to describe the tool
    QString getIconName() const override
    {
        return ":/tool/link.svg";
    }

    QString getCategory() const override
    {
        return trUtf8("Stereo samples");
    }

    /// Internal identifier
    QString getIdentifier() const override
    {
        return "smpl:link";
    }

    /// Method executed before the iterating process
    void beforeProcess(IdList ids) override;

    /// Process an element
    void process(SoundfontManager * sm, EltID id, AbstractToolParameters * parameters) override;

protected:
    QString getLabelInternal() const override
    {
        return trUtf8("Find a link");
    }

private:
    void scan(SoundfontManager * sm, int indexSf2);
    void associate(SoundfontManager * sm, EltID leftSmpl, EltID rightSmpl);
    void disconnectTarget(SoundfontManager * sm, EltID id);

    QList<int> _smplIds;
    QStringList _names;
    QList<bool> _leftRight; // false is left, true is right
    QList<int> _processedIds;
    QStringList _sampleErrors;
};

#endif // TOOLLINKSAMPLE_H
