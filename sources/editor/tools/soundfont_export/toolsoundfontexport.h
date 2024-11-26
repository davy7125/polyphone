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

#ifndef TOOLSOUNDFONTEXPORT_H
#define TOOLSOUNDFONTEXPORT_H

#include "abstracttoolonestep.h"

class ToolSoundfontExport: public AbstractToolOneStep
{
    Q_OBJECT

public:
    ToolSoundfontExport();
    virtual ~ToolSoundfontExport();

    /// Icon, label and category displayed to the user to describe the tool
    QString getIconName() const override
    {
        return ":/tool/file-export.svg";
    }

    QString getCategory() const override
    {
        return tr("Files");
    }

    /// Internal identification
    QString getToolName() const override
    {
        return "export";
    }

    /// Process asynchronously run
    void process(SoundfontManager * sm, IdList ids, AbstractToolParameters * parameters) override;

protected:
    QString getLabelInternal() const override
    {
        return tr("Export");
    }

    /// Return true if the tool can be used on the specified ids
    bool isCompatible(IdList ids) override;

    /// Get the warning to display after the tool is run
    QString getWarning() override;

    /// Get a confirmation message after the tool is run
    QString getConfirmation() override;

    virtual ElementType getCurrentType() const override
    {
        return elementSf2;
    }

private:
    void process(SoundfontManager * sm, QString name, QMap<int,  QList<int> > presets, AbstractToolParameters *parameters);
    EltID mergeSoundfonts(SoundfontManager * sm, QMap<int,  QList<int> > presets);
    QString getName(SoundfontManager * sm, QList<int> sf2Indexes);
    QString getFilePath(QString directory, QString name, int format);
    QString _error;
};

#endif // TOOLSOUNDFONTEXPORT_H
