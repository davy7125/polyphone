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

#include "toolfrequencyfilter.h"
#include "toolfrequencyfilter_gui.h"
#include "toolfrequencyfilter_parameters.h"
#include "soundfontmanager.h"
#include "gaincurve.h"

ToolFrequencyFilter::ToolFrequencyFilter() : AbstractToolIterating(elementSmpl, new ToolFrequencyFilter_parameters(), new ToolFrequencyFilter_gui())
{

}

void ToolFrequencyFilter::beforeProcess(IdList ids)
{
    Q_UNUSED(ids)
    QMutexLocker locker(&_mutex);
    _reports.clear();
}

void ToolFrequencyFilter::process(SoundfontManager * sm, EltID id, AbstractToolParameters *parameters)
{
    GainCurve curve = dynamic_cast<ToolFrequencyFilter_parameters *>(parameters)->getCurve();

    // Data, sample rate, fundamental frequency and loop of the sample
    QVector<float> vData = sm->getDataFloat(id);
    double sampleRate = sm->get(id, champ_dwSampleRate).dwValue;
    double fundamental = GainCurve::getFundamental(sm->get(id, champ_byOriginalPitch).bValue,
                                                   sm->get(id, champ_chPitchCorrection).cValue);
    quint32 loopStart = sm->get(id, champ_dwStartLoop).dwValue;
    quint32 loopEnd = sm->get(id, champ_dwEndLoop).dwValue;

    // Apply the filter and update data
    Report report;
    report.id = id;
    report.fundamental = curve.isNoteTracking() ? fundamental : 0;
    vData = curve.apply(vData, sampleRate, fundamental, loopStart, loopEnd, &report.peakBefore, &report.peakAfter);
    sm->setData(id, vData);

    QMutexLocker locker(&_mutex);
    _reports << report;
}

QList<ToolFrequencyFilter::Report> ToolFrequencyFilter::getReports()
{
    QMutexLocker locker(&_mutex);
    return _reports;
}
