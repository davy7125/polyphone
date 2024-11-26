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

#ifndef TOOLGLOBALSETTINGS_PARAMETERS_H
#define TOOLGLOBALSETTINGS_PARAMETERS_H

#include "abstracttoolparameters.h"
#include "contextmanager.h"
#include <QVector>

class ToolGlobalSettings_parameters: public AbstractToolParameters
{
public:
    /// Load the configuration from the ini file
    void loadConfiguration() override;

    /// Save the configuration in the ini file
    void saveConfiguration() override;

    int getPattern() { return _pattern; }
    void setPattern(int pattern) { _pattern = pattern; }

    double getParam() { return _param; }
    void setParam(double param) { _param = param; }

    double getMin() { return qMin(_min, _max); }
    void setMin(double min) { _min = min; }

    double getMax() { return qMax(_min, _max); }
    void setMax(double max) { _max = max; }

    double getMinX() { return _minX; }
    void setMinX(double minX) { _minX = minX; }

    double getMaxX() { return _maxX; }
    void setMaxX(double maxX) { _maxX = maxX; }

    QVector<float> getValues() { return _values; }
    void setValues(QVector<float> values) { _values = values; }

    int getModifType() { return _modifType; }
    void setModifType(int modifType) { _modifType = modifType; }

    int getAttribute() { return _attribute; }
    void setAttribute(int attribute) { _attribute = attribute; }

    int getMinVel() { return qMin(_minVel, _maxVel); }
    void setMinVel(int minVel) { _minVel = minVel; }

    int getMaxVel() { return qMax(_minVel, _maxVel); }
    void setMaxVel(int maxVel) { _maxVel = maxVel; }

private:
    QVector<float> getStoredValues();
    void storeValues(QVector<float> values);

    int _pattern;
    double _param;
    double _min;
    double _max;
    double _minX;
    double _maxX;

    QVector<float> _values;

    int _modifType;
    int _attribute;
    int _minVel;
    int _maxVel;
};

#endif // TOOLGLOBALSETTINGS_PARAMETERS_H
