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

#ifndef TOOLFREQUENCYFILTER_PARAMETERS_H
#define TOOLFREQUENCYFILTER_PARAMETERS_H

#include "abstracttoolparameters.h"
#include "gaincurve.h"

class ToolFrequencyFilter_parameters: public AbstractToolParameters
{
public:
    ToolFrequencyFilter_parameters();

    /// Load the configuration from the ini file
    void loadConfiguration() override;

    /// Save the configuration in the ini file
    void saveConfiguration() override;

    GainCurve::Mode getMode() const { return _mode; }
    void setMode(GainCurve::Mode mode) { _mode = mode; }

    bool isNoteTracking() const { return _noteTracking; }
    void setNoteTracking(bool isEnabled) { _noteTracking = isEnabled; }

    /// Points of the current mode and abscissa, each combination keeping its own curve
    QList<QPointF> getPoints() const { return _points[getIndex(_mode, _noteTracking)]; }
    void setPoints(const QList<QPointF> &points) { _points[getIndex(_mode, _noteTracking)] = points; }
    QList<QPointF> getPoints(GainCurve::Mode mode, bool noteTracking) const { return _points[getIndex(mode, noteTracking)]; }
    void setPoints(GainCurve::Mode mode, bool noteTracking, const QList<QPointF> &points) { _points[getIndex(mode, noteTracking)] = points; }

    GainCurve getCurve() const { return GainCurve(_mode, _noteTracking, getPoints()); }

private:
    static int getIndex(GainCurve::Mode mode, bool noteTracking) { return 2 * static_cast<int>(mode) + (noteTracking ? 1 : 0); }

    GainCurve::Mode _mode;
    bool _noteTracking;
    QList<QPointF> _points[4];
    static const char * POINT_KEYS[4];
};

#endif // TOOLFREQUENCYFILTER_PARAMETERS_H
