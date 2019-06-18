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

#ifndef SFARKEXTRACTOR2_H
#define SFARKEXTRACTOR2_H

#include "abstractextractor.h"

class SfArkExtractor2: public AbstractExtractor
{
public:
    SfArkExtractor2(const char * fileName);
    virtual ~SfArkExtractor2();
    bool extract(const char * outputFilePath) override;
    virtual QString getError() override
    {
        QString error = "";
        switch (_errorNumber)
        {
        case 0: error = "No error"; break;
        case -1: error = "Failed to initialize"; break;
        case -2: error = "Failed to allocate memory"; break;
        case -3: error = "header does not contain 'sfArk' signature"; break;
        case -4: error = "sfArk file has a corrupt header"; break;
        case -5: error = "sfArk file is incompatible (i.e. not sfArk V2.x)"; break;
        case -6: error = "sfArk file uses unsupported feature"; break;
        case -7: error = "got invalid compressed data (file is corrupted)"; break;
        case -8: error = "file checksum failed (file is corrupted)"; break;
        case -9: error = "File i/o error"; break;
        case -10: error = "License included not agreed by user"; break;
        case -11: error = "Other error (currently unused)"; break;
        default: error = "Unknown error (" + QString::number(_error) + ")"; break;
        }
        return error;
    }

    static SfArkFileManager _fileManager;
private:
    bool _error;
    int _errorNumber;
    QString _filename;
};

#endif // SFARKEXTRACTOR2_H
