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

#ifndef INTERACTIVESAMPLELOADER_H
#define INTERACTIVESAMPLELOADER_H

#include "sampleloader.h"
class QWidget;

// Samples loaded by the user: processing of the configuration, recent files, errors shown and questions asked about
// the samples having the same name as existing ones
class InteractiveSampleLoader: public SampleLoader
{
public:
    InteractiveSampleLoader(QWidget * parent = nullptr);

protected:
    void loading(QString path) override;
    void showError(QString error) override;
    int askReplacement(QString question, int replace) override;

private:
    QWidget * _parent;
};

#endif // INTERACTIVESAMPLELOADER_H
