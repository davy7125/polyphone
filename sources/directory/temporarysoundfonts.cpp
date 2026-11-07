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

#include "temporarysoundfonts.h"
#include <QTimer>
#include <QCoreApplication>
#include "contextmanager.h"
#include "soundfontmanager.h"
#include "synth.h"

TemporarySoundfonts * TemporarySoundfonts::s_instance = nullptr;
const int TemporarySoundfonts::CHECK_INTERVAL_MS = 100;
const int TemporarySoundfonts::VOICE_RELEASE_DELAY_MS = 10000;

TemporarySoundfonts::TemporarySoundfonts() : QObject(QCoreApplication::instance()),
    _timer(new QTimer(this))
{
    _clock.start();
    _timer->setInterval(CHECK_INTERVAL_MS);
    connect(_timer, SIGNAL(timeout()), this, SLOT(check()));
}

TemporarySoundfonts * TemporarySoundfonts::getInstance()
{
    if (s_instance == nullptr)
        s_instance = new TemporarySoundfonts();
    return s_instance;
}

void TemporarySoundfonts::hold(int sf2Index)
{
    if (sf2Index < 0)
        return;
    TemporarySoundfonts * instance = getInstance();
    instance->_holdCounts[sf2Index]++;
    instance->_releaseTimes.remove(sf2Index);
}

void TemporarySoundfonts::release(int sf2Index)
{
    TemporarySoundfonts * instance = getInstance();
    if (!instance->_holdCounts.contains(sf2Index))
        return;
    if (--instance->_holdCounts[sf2Index] > 0)
        return;
    instance->_holdCounts.remove(sf2Index);

    // Deleted later, never during the call (the soundfont manager notifies the closure)
    instance->_releaseTimes[sf2Index] = instance->_clock.elapsed();
    if (!instance->_timer->isActive())
        instance->_timer->start();
}

void TemporarySoundfonts::check()
{
    SoundfontManager * sm = SoundfontManager::getInstance();
    Synth * synth = ContextManager::audio()->getSynth();
    foreach (int sf2Index, _releaseTimes.keys())
    {
        // Nothing to delete if the soundfont has already been closed or if a tab has been opened with it
        EltID idSf2(elementSf2, sf2Index);
        if (!sm->isValid(idSf2) || !sm->isTemporary(sf2Index))
        {
            _releaseTimes.remove(sf2Index);
            continue;
        }

        // Deleted when its voices are over (they read the sample data without copy)
        if (synth == nullptr || !synth->hasVoices(sf2Index))
        {
            _releaseTimes.remove(sf2Index);
            sm->remove(idSf2);
            continue;
        }

        // Notes possibly sustained without end
        if (_clock.elapsed() - _releaseTimes[sf2Index] > VOICE_RELEASE_DELAY_MS)
        {
            synth->play(idSf2, -2, -2, 0);
            _releaseTimes[sf2Index] = _clock.elapsed();
        }
    }

    if (_releaseTimes.isEmpty())
        _timer->stop();
}
