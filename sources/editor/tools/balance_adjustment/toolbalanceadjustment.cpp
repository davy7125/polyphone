/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2020 Davy Triponney                                **
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

#include "toolbalanceadjustment.h"
#include "soundfontmanager.h"
#include "sampleutils.h"
#include <qmath.h>

void ToolBalanceAdjustment::beforeProcess(IdList ids)
{
    Q_UNUSED(ids)
    _processedSamples.clear();
    _monoSamplesInError.clear();
}

void ToolBalanceAdjustment::process(SoundfontManager * sm, EltID id, AbstractToolParameters *parameters)
{
    Q_UNUSED(parameters);
    EltID id2 = id;

    // Sample already processed?
    _mutex.lock();
    if (_processedSamples.contains(id))
    {
        _mutex.unlock();
        return;
    }
    else
    {
        _processedSamples << id;

        // The sample must be linked
        SFSampleLink typeLien = sm->get(id, champ_sfSampleType).sfLinkValue;
        if (typeLien == monoSample || typeLien == RomMonoSample)
        {
            _monoSamplesInError << sm->getQstr(id, champ_name);
            _mutex.unlock();
            return;
        }
        else
        {
            id2.indexElt = sm->get(id, champ_wSampleLink).wValue;
            if (sm->isValid(id2))
                _processedSamples << id2;
            else
            {
                _monoSamplesInError << sm->getQstr(id, champ_name);
                _mutex.unlock();
                return;
            }
        }
    }
    _mutex.unlock();

    // Get sample data
    QVector<float> vData1 = sm->getData(id);
    QVector<float> vData2 = sm->getData(id2);

    // Find steady areas
    quint32 debut1, fin1;
    if (sm->get(id, champ_dwStartLoop).dwValue == sm->get(id, champ_dwEndLoop).dwValue)
        SampleUtils::regimePermanent(vData1, sm->get(id, champ_dwSampleRate).dwValue, debut1, fin1);
    else
    {
        debut1 = sm->get(id, champ_dwStartLoop).dwValue;
        fin1 = sm->get(id, champ_dwEndLoop).dwValue;
    }
    quint32 debut2, fin2;
    if (sm->get(id2, champ_dwStartLoop).dwValue == sm->get(id2, champ_dwEndLoop).dwValue)
        SampleUtils::regimePermanent(vData2, sm->get(id2, champ_dwSampleRate).dwValue, debut2, fin2);
    else
    {
        debut2 = sm->get(id2, champ_dwStartLoop).dwValue;
        fin2 = sm->get(id2, champ_dwEndLoop).dwValue;
    }

    // Compute intensities
    float intensite1 = SampleUtils::meanSquare(vData1.mid(debut1, fin1 - debut1));
    float intensite2 = SampleUtils::meanSquare(vData2.mid(debut2, fin2 - debut2));

    // Mean intensity
    float intensiteMoy = sqrt(intensite1 * intensite2);

    // Adjust volume
    float gain1, gain2;
    vData1 = SampleUtils::multiply(vData1, intensiteMoy / intensite1, gain1);
    vData2 = SampleUtils::multiply(vData2, intensiteMoy / intensite2, gain2);

    // Update sample data
    sm->set(id, vData1);
    sm->set(id2, vData2);
}

QString ToolBalanceAdjustment::getWarning()
{
    QString txt;

    if (!_monoSamplesInError.isEmpty())
    {
        txt = tr("This tool cannot be used with mono samples:");
        txt += "<ul>";
        for (int i = 0; i < _monoSamplesInError.size(); i++)
            txt += "<li>" + _monoSamplesInError.at(i) + "</li>";
        txt += "</ul>";
    }

    return txt;
}
