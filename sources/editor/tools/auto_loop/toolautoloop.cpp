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

#include "auto_loop/toolautoloop.h"
#include "soundfontmanager.h"
#include "sampleutils.h"

void ToolAutoLoop::beforeProcess(IdList ids)
{
    Q_UNUSED(ids)
    _processedSamples.clear();
    _samplesNotLooped.clear();
}

void ToolAutoLoop::process(SoundfontManager * sm, EltID id, AbstractToolParameters *parameters)
{
    Q_UNUSED(parameters)
    bool withLink = false;
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

        // The sample may be linked
        SFSampleLink typeLien = sm->get(id, champ_sfSampleType).sfLinkValue;
        if (typeLien != monoSample && typeLien != RomMonoSample)
        {
            id2.indexElt = sm->get(id, champ_wSampleLink).wValue;
            if (this->isProcessed(id2))
            {
                withLink = true;
                _processedSamples << id2;
            }
        }
    }
    _mutex.unlock();

    // Get data, sample rate, start and end loop of sample 1
    QVector<float> vData = sm->getDataFloat(id);
    quint32 dwSmplRate = sm->get(id, champ_dwSampleRate).dwValue;
    quint32 startLoop = sm->get(id, champ_dwStartLoop).dwValue;
    quint32 endLoop = sm->get(id, champ_dwEndLoop).dwValue;

    // Loop
    quint32 crossfadeLength;
    bool result = SampleUtils::loopStep1(vData, dwSmplRate, startLoop, endLoop, crossfadeLength);

    // Same for sample 2
    if (withLink)
    {
        // Only if the sample rates match
        quint32 dwSmplRate2 = sm->get(id2, champ_dwSampleRate).dwValue;
        if (dwSmplRate2 == dwSmplRate)
        {
            QVector<float> vData2 = sm->getDataFloat(id2);
            quint32 startLoop2 = sm->get(id2, champ_dwStartLoop).dwValue;
            quint32 endLoop2 = sm->get(id2, champ_dwEndLoop).dwValue;

            quint32 crossfadeLength2;
            bool result2 = SampleUtils::loopStep1(vData2, dwSmplRate2, startLoop2, endLoop2, crossfadeLength2);

            if (result && !result2)
            {
                // Take the values of sample 1, if possible
                if (endLoop + 8 < (unsigned int)vData2.size())
                    updateSample(id2, vData2, startLoop, endLoop, crossfadeLength);
                else
                {
                    // Sample 2 cannot be processed
                    _mutex.lock();
                    _samplesNotLooped << sm->getQstr(id2, champ_name);
                    _mutex.unlock();
                }
            }
            else if (!result && result2)
            {
                // Use the values of sample 2 for sample 1, if possible
                if (endLoop2 + 8 < (unsigned int)vData.size())
                {
                    startLoop = startLoop2;
                    endLoop = endLoop2;
                    crossfadeLength = crossfadeLength2;
                    result = true;
                }

                // Update sample 2
                updateSample(id2, vData2, startLoop2, endLoop2, crossfadeLength2);
            }
            else if (!result && !result2)
            {
                // None of the samples could be looped
                _mutex.lock();
                _samplesNotLooped << sm->getQstr(id2, champ_name);
                _mutex.unlock();
            }
            else
            {
                // Use the longest possible loop for both samples
                if (endLoop < endLoop2)
                {
                    if (endLoop2 + 8 < (unsigned int)vData.size())
                    {
                        // Use the values of sample 2 for both samples (long loop)
                        startLoop = startLoop2;
                        endLoop = endLoop2;
                        crossfadeLength = crossfadeLength2;
                    }
                    else
                    {
                        // Use the values of sample 1 for both samples (short loop)
                        startLoop2 = startLoop;
                        endLoop2 = endLoop;
                        crossfadeLength2 = crossfadeLength;
                    }
                }
                else
                {
                    if (endLoop + 8 < (unsigned int)vData2.size())
                    {
                        // Use the values of sample 1 for both samples (long loop)
                        startLoop2 = startLoop;
                        endLoop2 = endLoop;
                        crossfadeLength2 = crossfadeLength;
                    }
                    else
                    {
                        // Use the values of sample 2 for both samples (short loop)
                        startLoop = startLoop2;
                        endLoop = endLoop2;
                        crossfadeLength = crossfadeLength2;
                    }
                }

                // Update sample 2
                updateSample(id2, vData2, startLoop2, endLoop2, crossfadeLength2);
            }
        }
    }

    if (result)
        updateSample(id, vData, startLoop, endLoop, crossfadeLength);
    else
    {
        _mutex.lock();
        _samplesNotLooped << sm->getQstr(id, champ_name);
        _mutex.unlock();
    }
}

void ToolAutoLoop::updateSample(EltID id, QVector<float> vData, quint32 startLoop, quint32 endLoop, quint32 crossfadeLength)
{
    SoundfontManager * sm = SoundfontManager::getInstance();

    // Update data
    vData = SampleUtils::loopStep2(vData, startLoop, endLoop, crossfadeLength, true);
    sm->set(id, vData);

    // Update length, startloop and endloop
    AttributeValue val;
    val.dwValue = startLoop;
    sm->set(id, champ_dwStartLoop, val);
    val.dwValue = endLoop;
    sm->set(id, champ_dwEndLoop, val);
    val.dwValue = static_cast<quint32>(vData.size());
    sm->set(id, champ_dwLength, val);
}

QString ToolAutoLoop::getWarning()
{
    QString txt;

    if (!_samplesNotLooped.isEmpty())
    {

        if (_samplesNotLooped.size() == 1)
            txt = tr("Failed to loop sample \"%1\".").arg(_samplesNotLooped.first()) + "<br/>";
        else
        {
            txt = tr("The following samples couldn't be looped:") + "<ul>";
            for (int i = 0; i < _samplesNotLooped.size(); i++)
                txt += "<li>" + _samplesNotLooped.at(i) + "</li>";
            txt += "</ul>";
        }
        txt += tr("Possible reasons: too short or too turbulent.");
    }

    return txt;
}
