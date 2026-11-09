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

#ifndef SAMPLELOADER_H
#define SAMPLELOADER_H

#include "basetypes.h"
#include <QList>

class SoundfontManager;

// Audio files loaded as samples of a soundfont, without interaction: the user interface derives this class to show
// the errors and to ask what to do with the samples having the same name as existing ones (InteractiveSampleLoader)
class SampleLoader
{
public:
    SampleLoader();
    virtual ~SampleLoader() {}

    /// Processing of the samples loaded: data removed after the end of the loop, blank removed at the start
    void setProcessing(bool trimToLoop, bool trimStart);

    /// Add a sample to a soundfont
    /// "replace" is what to do with a sample having the same name as an existing one, possibly changed by the user:
    /// -1: duplicate all, 0: duplicate, 1: replace, 2: replace all, 3: ignore, 4: ignore all
    IdList load(QString path, int numSf2, int *replace);

protected:
    /// Called before a file is loaded
    virtual void loading(QString path) { Q_UNUSED(path) }

    /// The file cannot be read
    virtual void showError(QString error) { Q_UNUSED(error) }

    /// A sample has the same name as an existing one, "replace" being 0, 1 or 3: return what to do (same values as
    /// "replace"). Without interaction, the current choice applies.
    virtual int askReplacement(QString question, int replace) { Q_UNUSED(question) return replace; }

private:
    QString getName(QString name, int maxCharacters, int suffixNumber);

    SoundfontManager * _sm;
    bool _trimToLoop;
    bool _trimStart;
};

#endif // SAMPLELOADER_H
