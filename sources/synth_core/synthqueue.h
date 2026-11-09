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

#ifndef SYNTHQUEUE_H
#define SYNTHQUEUE_H

#include <atomic>
#include <cstdint>
#include <memory>

// Queue between two threads, without lock nor allocation once created: one thread pushes the items (for instance the
// interface of a plugin, so that it never calls the synth directly), another one pops them (the audio thread, before
// computing). The capacity is fixed.
template <class T>
class SynthQueue
{
public:
    // Capacity rounded up to a power of 2
    explicit SynthQueue(uint32_t capacity)
    {
        uint32_t size = 2;
        while (size < capacity && size < 0x80000000u)
            size <<= 1;
        _items.reset(new T[size]);
        _mask = size - 1;
    }

    SynthQueue(const SynthQueue &) = delete;
    SynthQueue &operator=(const SynthQueue &) = delete;

    // Pushing thread: false if the queue is full (the item is not added)
    bool push(const T &item)
    {
        uint32_t writeIndex = _writeIndex.load(std::memory_order_relaxed);
        if (writeIndex - _readIndex.load(std::memory_order_acquire) > _mask)
            return false;
        _items[writeIndex & _mask] = item;
        _writeIndex.store(writeIndex + 1, std::memory_order_release);
        return true;
    }

    // Pushing thread: true if the next push would fail
    bool isFull() const
    {
        return _writeIndex.load(std::memory_order_relaxed) - _readIndex.load(std::memory_order_acquire) > _mask;
    }

    // Popping thread: false if the queue is empty
    bool pop(T &item)
    {
        uint32_t readIndex = _readIndex.load(std::memory_order_relaxed);
        if (readIndex == _writeIndex.load(std::memory_order_acquire))
            return false;
        item = _items[readIndex & _mask];
        _readIndex.store(readIndex + 1, std::memory_order_release);
        return true;
    }

private:
    std::unique_ptr<T[]> _items;
    uint32_t _mask;

    // Each index on its own cache line, written by one thread
    alignas(64) std::atomic<uint32_t> _writeIndex { 0 };
    alignas(64) std::atomic<uint32_t> _readIndex { 0 };
};

#endif // SYNTHQUEUE_H
