/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2026 Davy Triponney                                **
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

#include "synthsoundfontset.h"
#include "synthcore.h"

SynthSoundfontSet::SynthSoundfontSet(int capacity) :
    _slots(new Slot[capacity > 0 ? capacity : 1]),
    _capacity(capacity > 0 ? capacity : 1)
{
    for (int i = 0; i < _capacity; i++)
    {
        _slots[i].state.store(SLOT_FREE, std::memory_order_relaxed);
        _slots[i].pointer = nullptr;
    }
}

SynthSoundfontSet::~SynthSoundfontSet()
{}

int SynthSoundfontSet::add(std::shared_ptr<ISynthSoundfont> soundfont)
{
    // The soundfont is complete for the audio thread once it reads the state SLOT_PLAYED
    for (int i = 0; i < _capacity; i++)
    {
        if (_slots[i].state.load(std::memory_order_acquire) == SLOT_FREE)
        {
            _slots[i].pointer = soundfont.get();
            _slots[i].soundfont = std::move(soundfont);
            _slots[i].state.store(SLOT_PLAYED, std::memory_order_release);
            return i;
        }
    }
    return -1;
}

void SynthSoundfontSet::remove(int index)
{
    if (index < 0 || index >= _capacity)
        return;
    int expected = SLOT_PLAYED;
    _slots[index].state.compare_exchange_strong(expected, SLOT_REMOVED, std::memory_order_relaxed);
}

void SynthSoundfontSet::checkRemoved(SynthCore &core)
{
    // The audio thread does not read the soundfont anymore once it writes the state SLOT_UNUSED
    for (int i = 0; i < _capacity; i++)
        if (_slots[i].state.load(std::memory_order_relaxed) == SLOT_REMOVED && !core.hasVoices(i))
            _slots[i].state.store(SLOT_UNUSED, std::memory_order_release);
}

int SynthSoundfontSet::deleteUnused()
{
    int count = 0;
    for (int i = 0; i < _capacity; i++)
    {
        if (_slots[i].state.load(std::memory_order_acquire) == SLOT_UNUSED)
        {
            _slots[i].soundfont.reset();
            _slots[i].pointer = nullptr;
            _slots[i].state.store(SLOT_FREE, std::memory_order_release);
            count++;
        }
    }
    return count;
}

ISynthSoundfont * SynthSoundfontSet::getSynthSoundfont(int index)
{
    if (index < 0 || index >= _capacity || _slots[index].state.load(std::memory_order_acquire) != SLOT_PLAYED)
        return nullptr;
    return _slots[index].pointer;
}
