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

#ifndef SYNTHSOUNDFONTSET_H
#define SYNTHSOUNDFONTSET_H

#include <atomic>
#include <memory>
#include "isynthsoundfont.h"
class SynthCore;

// Soundfonts of a synth whose audio thread never waits (plugin), replaced while the audio goes on. They are added and
// deleted by one thread, the "owner" (for instance the one loading the files), while the audio thread plays them:
// 1. the owner adds a soundfont, played at once with its index;
// 2. the audio thread removes it: it is not played anymore, and its voices are to be released quickly;
// 3. before each computation, the audio thread notes which removed soundfonts have no voice anymore;
// 4. the owner releases these ones, and their indexes can be used again.
// The set shares the ownership of the soundfonts added (a soundfont can be played by several synths, for instance
// instances of a plugin): it releases the remaining ones when it is deleted (when no audio is computed).
class SynthSoundfontSet: public ISynthSoundfonts
{
public:
    explicit SynthSoundfontSet(int capacity = 16);
    ~SynthSoundfontSet() override;

    // Owner: index of the soundfont added, -1 if all indexes are used
    int add(std::shared_ptr<ISynthSoundfont> soundfont);

    // Audio thread: the soundfont is not played anymore (getSynthSoundfont returns null for its index)
    void remove(int index);

    // Audio thread, before computing: the removed soundfonts that have no voice anymore can be deleted
    void checkRemoved(SynthCore &core);

    // Owner: release the soundfonts that are not used anymore (deleted if nothing else shares them), return their number
    int deleteUnused();

    // ISynthSoundfonts implementation (audio thread): null if no soundfont is played at this index
    ISynthSoundfont * getSynthSoundfont(int index) override;

private:
    enum SlotState
    {
        SLOT_FREE,
        SLOT_PLAYED,
        SLOT_REMOVED,
        SLOT_UNUSED
    };

    struct Slot
    {
        std::atomic<int> state;
        std::shared_ptr<ISynthSoundfont> soundfont; // Written by the owner in the state SLOT_FREE only
        ISynthSoundfont * pointer; // Read by the audio thread (the shared pointer is not copied by it)
    };

    std::unique_ptr<Slot[]> _slots;
    int _capacity;
};

#endif // SYNTHSOUNDFONTSET_H
