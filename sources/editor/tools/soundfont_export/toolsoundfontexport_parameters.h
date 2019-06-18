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

#ifndef TOOLSOUNDFONTEXPORT_PARAMETERS_H
#define TOOLSOUNDFONTEXPORT_PARAMETERS_H

#include "abstracttoolparameters.h"
#include "basetypes.h"

class ToolSoundfontExport_parameters: public AbstractToolParameters
{
public:
    /// Load the configuration from the ini file
    void loadConfiguration() override;

    /// Save the configuration in the ini file
    void saveConfiguration() override;

    QString getDirectory() { return _directory; }
    void setDirectory(QString directory) { _directory = directory; }

    int getFormat() { return _format; }
    void setFormat(int format) { _format = format; }

    int getQuality() { return _quality; }
    void setQuality(int quality) { _quality = quality; }

    bool getPresetPrefix() { return _presetPrefix; }
    void setPresetPrefix(bool presetPrefix) { _presetPrefix = presetPrefix; }

    bool getBankDirectory() { return _bankDirectory; }
    void setBankDirectory(bool bankDirectory) { _bankDirectory = bankDirectory; }

    bool getGmSort() { return _gmSort; }
    void setGmSort(bool gmSort) { _gmSort = gmSort; }
    
    // Transitional data
    QMap<int, QList<int> > & getSelectedPresets() { return _selectedPresets; }
    void setSelectedPresets(QMap<int, QList<int> > selectedPresets) { _selectedPresets = selectedPresets; }

private:
    QString _directory;
    int _format;
    int _quality;
    bool _presetPrefix;
    bool _bankDirectory;
    bool _gmSort;

    QMap<int, QList<int> > _selectedPresets;
};

#endif // TOOLSOUNDFONTEXPORT_PARAMETERS_H
