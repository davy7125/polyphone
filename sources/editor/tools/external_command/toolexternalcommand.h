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

#ifndef TOOLEXTERNALCOMMAND_H
#define TOOLEXTERNALCOMMAND_H

#include "abstracttooliterating.h"
#include <QObject>
#include "sound.h"
class QProcess;

class ToolExternalCommand: public AbstractToolIterating
{
    Q_OBJECT

public:
    ToolExternalCommand();

    /// Icon, label and category displayed to the user to describe the tool
    QString getIconName() const override
    {
        return ":/tool/command_line.svg";
    }

    QString getCategory() const override
    {
        return tr("Audio processing");
    }

    /// Internal identification
    QString getToolName() const override
    {
        return "command";
    }

    /// Method executed before the iterating process
    void beforeProcess(IdList ids) override;

    /// Process an element
    void process(SoundfontManager * sm, EltID id, AbstractToolParameters * parameters) override;

protected:
    QString getLabelInternal() const override
    {
        return tr("External command");
    }

    /// Get the warning to display after the tool is run
    QString getWarning() override;

private:
    void storeStereoIds(QList<EltID> ids);
    void import(EltID id, Sound &sound, SoundfontManager * sm, bool replaceInfo);

    /// All samples than have been processed
    QList<EltID> _processedIds;

    QString _warning;
};

#endif // TOOLEXTERNALCOMMAND_H
