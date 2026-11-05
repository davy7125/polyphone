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
    QVector<float> vData1 = sm->getDataFloat(id);
    QVector<float> vData2 = sm->getDataFloat(id2);

    // Steady area common to both channels: a loop, otherwise the steady part of their mix
    quint32 length = static_cast<quint32>(qMin(vData1.size(), vData2.size()));
    quint32 debut, fin;
    if (sm->get(id, champ_dwStartLoop).dwValue < sm->get(id, champ_dwEndLoop).dwValue)
    {
        debut = sm->get(id, champ_dwStartLoop).dwValue;
        fin = sm->get(id, champ_dwEndLoop).dwValue;
    }
    else if (sm->get(id2, champ_dwStartLoop).dwValue < sm->get(id2, champ_dwEndLoop).dwValue)
    {
        debut = sm->get(id2, champ_dwStartLoop).dwValue;
        fin = sm->get(id2, champ_dwEndLoop).dwValue;
    }
    else
    {
        // Absolute values mixed, so that opposite phases don't cancel
        QVector<float> mix(static_cast<int>(length));
        for (int i = 0; i < mix.size(); i++)
            mix[i] = 0.5f * (qAbs(vData1[i]) + qAbs(vData2[i]));
        SampleUtils::findSteadyPart(mix, sm->get(id, champ_dwSampleRate).dwValue, debut, fin);
    }
    fin = qMin(fin, length);
    if (fin <= debut)
    {
        debut = 0;
        fin = length;
    }

    // Compute intensities
    float intensite1 = SampleUtils::rms(vData1.constData() + debut, fin - debut);
    float intensite2 = SampleUtils::rms(vData2.constData() + debut, fin - debut);
    if (intensite1 <= 0 || intensite2 <= 0)
        return; // A silent channel cannot be balanced

    // Mean intensity
    float intensiteMoy = sqrt(intensite1 * intensite2);

    // Adjust volume
    float gain1, gain2;
    vData1 = SampleUtils::multiply(std::move(vData1), intensiteMoy / intensite1, gain1);
    vData2 = SampleUtils::multiply(std::move(vData2), intensiteMoy / intensite2, gain2);

    // Update sample data
    sm->setData(id, vData1);
    sm->setData(id2, vData2);
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
