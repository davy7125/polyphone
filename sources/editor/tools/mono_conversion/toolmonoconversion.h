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

#ifndef TOOLMONOCONVERSION_H
#define TOOLMONOCONVERSION_H

#include "abstracttooliterating.h"
#include <QObject>
class SoundfontManager;

class ToolMonoConversion: public AbstractToolIterating
{
    Q_OBJECT

public:
    ToolMonoConversion();

    /// Icon, label and category displayed to the user to describe the tool
    QString getIconName() const override
    {
        return ":/tool/mono_conversion.svg";
    }

    QString getCategory() const override
    {
        return tr("Stereo samples");
    }

    /// Internal identification
    QString getToolName() const override
    {
        return "monoConversion";
    }

    /// Method executed before the iterating process
    void beforeProcess(IdList ids) override;

    /// Process an element
    void process(SoundfontManager * sm, EltID id, AbstractToolParameters * parameters) override;

    /// Return true if the sample is linked to another valid sample, stored in idLinked
    static bool getLinkedSample(SoundfontManager * sm, EltID idSmpl, EltID &idLinked);

protected:
    QString getLabelInternal() const override
    {
        return tr("Convert to mono");
    }

    /// Get an error preventing the tool from running on the ids to process
    QString getBlockingError(IdList ids) override;

    /// Get the warning to display after the tool is run
    QString getWarning() override;

private:
    void updateInstruments(SoundfontManager * sm, EltID idLeft, EltID idRight, EltID idKept, bool merge);
    void mergeDivisions(SoundfontManager * sm, EltID idDivLeft, EltID idDivRight);
    void mergeSplitAttribute(SoundfontManager * sm, EltID idDivLeft, EltID idDivRight,
                             AttributeType champFine, AttributeType champCoarse, int coarseFactor);

    /// Value of a parameter in a division, possibly inherited from the global division or the default value
    static AttributeValue getValue(SoundfontManager * sm, EltID idDiv, AttributeType champ);

    /// Name of the left sample without the L / R suffix, if the names differ only by this suffix
    static QString getMonoName(QString leftName, QString rightName);

    static QString getHtmlList(QStringList elements);

    IdList _processedSamples;
    QStringList _sampleRateErrors;
    QStringList _clippedSamples;
    QStringList _unmergedInstruments;
};

#endif // TOOLMONOCONVERSION_H
