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

#include "toolfrequencyfilter.h"
#include "toolfrequencyfilter_gui.h"
#include "toolfrequencyfilter_parameters.h"
#include "soundfontmanager.h"
#include "sampleutils.h"

ToolFrequencyFilter::ToolFrequencyFilter() : AbstractToolIterating(elementSmpl, new ToolFrequencyFilter_parameters(), new ToolFrequencyFilter_gui())
{

}

void ToolFrequencyFilter::process(SoundfontManager * sm, EltID id, AbstractToolParameters *parameters)
{
    // Get the curve used to cut frequencies
    ToolFrequencyFilter_parameters * params = (ToolFrequencyFilter_parameters *)parameters;
    QVector<double> dValues = params->getCurve();

    // Get data and sample rate of the sample
    QByteArray baData = sm->getData(id, champ_sampleDataFull24);
    quint32 dwSmplRate = sm->get(id, champ_dwSampleRate).dwValue;

    // Apply the filter and update data
    baData = SampleUtils::cutFilter(baData, dwSmplRate, dValues, 24, 20000);
    sm->set(id, champ_sampleDataFull24, baData);
}
