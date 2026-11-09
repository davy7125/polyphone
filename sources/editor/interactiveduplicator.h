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

#ifndef INTERACTIVEDUPLICATOR_H
#define INTERACTIVEDUPLICATOR_H

#include "duplicator.h"
class QWidget;

// Copy of elements by the user: questions asked about the elements having the same name as existing ones, warnings
// shown (the warning about the global division possibly disabled in the configuration)
class InteractiveDuplicator: public Duplicator
{
public:
    InteractiveDuplicator();

protected:
    EtatMessage askReplacement(QString question) override;
    void warnGlobalNotCopied() override;
    void warnNoPresetAvailable() override;

private:
    QWidget * _parent;
};

#endif // INTERACTIVEDUPLICATOR_H
