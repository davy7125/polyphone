/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2016 Davy Triponney                                **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: http://polyphone-soundfonts.com                      **
**             Date: 01.01.2013                                           **
***************************************************************************/

#ifndef SFARKEXTRACTOR_H
#define SFARKEXTRACTOR_H

#include "abstractextractor.h"

class QWidget;

class SfArkExtractor : public AbstractExtractor
{
    Q_OBJECT

public:
    SfArkExtractor(const char * fileName, QWidget * parent);
    virtual ~SfArkExtractor();
    virtual void extract();
    virtual bool getData(char *&data, qint32 &size);

private:
    AbstractExtractor * _extractor;
    QWidget * _parent;

    void extract2();
};

#endif // SFARKEXTRACTOR_H
