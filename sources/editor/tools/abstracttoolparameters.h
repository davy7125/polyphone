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

#ifndef ABSTRACTTOOLPARAMETERS_H
#define ABSTRACTTOOLPARAMETERS_H

#include "contextmanager.h"
#include <QVariant>

class AbstractToolParameters
{
public:
    AbstractToolParameters() {}
    virtual ~AbstractToolParameters() {}

    /// Set the configuration for loading / saving parameters
    void setConfigurationSubSection(QString configurationSubSection)
    {
        _configurationSubSection = configurationSubSection;
    }

    /// Load the configuration from the ini file
    virtual void loadConfiguration() = 0;

    /// Save the configuration in the ini file
    virtual void saveConfiguration() = 0;

protected:
    QVariant getToolValue(QString key, QVariant defaultValue)
    {
        return ContextManager::configuration()->getValue(ConfManager::SECTION_TOOLS, _configurationSubSection, key, defaultValue);
    }

    void setToolValue(QString key, QVariant value)
    {
        ContextManager::configuration()->setValue(ConfManager::SECTION_TOOLS, _configurationSubSection, key, value);
    }

private:
    QString _configurationSubSection;
};

#endif // ABSTRACTTOOLPARAMETERS_H
