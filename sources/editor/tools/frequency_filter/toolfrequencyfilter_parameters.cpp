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

#include "toolfrequencyfilter_parameters.h"
#include "contextmanager.h"
#include <qmath.h>

// Indexed by 2 * mode + note tracking
const char * ToolFrequencyFilter_parameters::POINT_KEYS[4] = {
    "points_ceiling_hz", "points_ceiling_note", "points_gain_hz", "points_gain_note"
};

ToolFrequencyFilter_parameters::ToolFrequencyFilter_parameters() :
    _mode(GainCurve::MODE_CEILING),
    _noteTracking(false)
{}

void ToolFrequencyFilter_parameters::loadConfiguration()
{
    _mode = getToolValue("mode", 0).toInt() == 1 ? GainCurve::MODE_GAIN : GainCurve::MODE_CEILING;
    _noteTracking = getToolValue("note_tracking", false).toBool();
    for (int i = 0; i < 4; i++)
        _points[i] = GainCurve::pointsFromVariant(getToolValue(POINT_KEYS[i], QVariantList()));

    // Curve of the previous versions: 201 fractions of the strongest module from 0 to 20 kHz, linearly interpolated
    if (_points[0].isEmpty())
    {
        QList<QVariant> previousCurve = getToolValue("curve", QList<QVariant>()).toList();
        for (int i = 1; i < previousCurve.size(); i++)
        {
            double value = previousCurve.at(i).toDouble();
            _points[0] << QPointF(i * 20000.0 / previousCurve.size(),
                                  value <= 0 ? GainCurve::CEILING_REMOVED - 20 : qMin(0.0, 20 * log10(value)));
        }
    }
}

void ToolFrequencyFilter_parameters::saveConfiguration()
{
    setToolValue("mode", static_cast<int>(_mode));
    setToolValue("note_tracking", _noteTracking);
    for (int i = 0; i < 4; i++)
        setToolValue(POINT_KEYS[i], GainCurve(GainCurve::MODE_GAIN, false, _points[i]).pointsToVariant());
}
