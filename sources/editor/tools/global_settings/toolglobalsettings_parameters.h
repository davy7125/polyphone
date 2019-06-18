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

    int getInstPattern() { return _instPattern; }
    void setInstPattern(int instPattern) { _instPattern = instPattern; }

    int getPrstPattern() { return _prstPattern; }
    void setPrstPattern(int prstPattern) { _prstPattern = prstPattern; }

    double getInstParam() { return _instParam; }
    void setInstParam(double instParam) { _instParam = instParam; }

    double getPrstParam() { return _prstParam; }
    void setPrstParam(double prstParam) { _prstParam = prstParam; }

    double getInstMin() { return qMin(_instMin, _instMax); }
    void setInstMin(double instMin) { _instMin = instMin; }

    double getPrstMin() { return qMin(_prstMin, _prstMax); }
    void setPrstMin(double prstMin) { _prstMin = prstMin; }

    double getInstMax() { return qMax(_instMin, _instMax); }
    void setInstMax(double instMax) { _instMax = instMax; }

    double getPrstMax() { return qMax(_prstMin, _prstMax); }
    void setPrstMax(double prstMax) { _prstMax = prstMax; }

    double getInstMinX() { return _instMinX; }
    void setInstMinX(double instMinX) { _instMinX = instMinX; }

    double getPrstMinX() { return _prstMinX; }
    void setPrstMinX(double prstMinX) { _prstMinX = prstMinX; }

    double getInstMaxX() { return _instMaxX; }
    void setInstMaxX(double instMaxX) { _instMaxX = instMaxX; }

    double getPrstMaxX() { return _prstMaxX; }
    void setPrstMaxX(double prstMaxX) { _prstMaxX = prstMaxX; }

    QVector<double> getInstValues() { return _instValues; }
    void setInstValues(QVector<double> instValues) { _instValues = instValues; }

    QVector<double> getPrstValues() { return _prstValues; }
    void setPrstValues(QVector<double> prstValues) { _prstValues = prstValues; }

    int getInstModifType() { return _instModifType; }
    void setInstModifType(int instModifType) { _instModifType = instModifType; }

    int getPrstModifType() { return _prstModifType; }
    void setPrstModifType(int prstModifType) { _prstModifType = prstModifType; }

    int getInstAttribute() { return _instAttribute; }
    void setInstAttribute(int instAttribute) { _instAttribute = instAttribute; }

    int getPrstAttribute() { return _prstAttribute; }
    void setPrstAttribute(int prstAttribute) { _prstAttribute = prstAttribute; }

    int getInstMinVel() { return qMin(_instMinVel, _instMaxVel); }
    void setInstMinVel(int instMinVel) { _instMinVel = instMinVel; }

    int getPrstMinVel() { return qMin(_prstMinVel, _prstMaxVel); }
    void setPrstMinVel(int prstMinVel) { _prstMinVel = prstMinVel; }

    int getInstMaxVel() { return qMax(_instMinVel, _instMaxVel); }
    void setInstMaxVel(int instMaxVel) { _instMaxVel = instMaxVel; }

    int getPrstMaxVel() { return qMax(_prstMinVel, _prstMaxVel); }
    void setPrstMaxVel(int prstMaxVel) { _prstMaxVel = prstMaxVel; }

private:
    static QVector<double> getStoredValues(ConfManager::ToolType toolType);
    static void storeValues(QVector<double> values, ConfManager::ToolType toolType);

    int _instPattern, _prstPattern;
    double _instParam, _prstParam;
    double _instMin, _prstMin;
    double _instMax, _prstMax;
    double _instMinX, _prstMinX;
    double _instMaxX, _prstMaxX;

    QVector<double> _instValues, _prstValues;

    int _instModifType, _prstModifType;
    int _instAttribute, _prstAttribute;
    int _instMinVel, _prstMinVel;
    int _instMaxVel, _prstMaxVel;
};

#endif // TOOLGLOBALSETTINGS_PARAMETERS_H
