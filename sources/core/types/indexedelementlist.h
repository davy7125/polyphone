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

#ifndef INDEXEDELEMENTLIST_H
#define INDEXEDELEMENTLIST_H

#include <QVector>

template <class T>
class IndexedElementList
{
public:
    // Add an element and return the index
    int add(T elt)
    {
        // Add the element in both vectors
        _elementsByIndex.append(elt);
        _elementsByPosition.append(elt);

        // Return the index
        return _elementsByIndex.count() - 1;
    }

    // Take an element at a specific index
    T takeAtIndex(int index)
    {
        T elt = _elementsByIndex[index];
        _elementsByIndex[index] = nullptr;
        _elementsByPosition.removeOne(elt);
        return elt;
    }

    // Get an element by index or position
    T atIndex(int index) const
    {
        return _elementsByIndex[index];
    }

    T atPosition(int position) const
    {
        return _elementsByPosition[position];
    }

    // Get the number of elements
    int positionCount() const
    {
        return _elementsByPosition.count();
    }

    // Get the number of indexes
    int indexCount() const
    {
        return _elementsByIndex.count();
    }

    // Return all values for iterating over them
    QVector<T> values() const { return _elementsByPosition; }

    // Get the position corresponding to an index
    int positionOfIndex(int index)
    {
        if (index < 0 || index >= _elementsByIndex.count())
            return -1;
        return _elementsByPosition.indexOf(_elementsByIndex[index]);
    }

private:
    QVector<T> _elementsByIndex;
    QVector<T> _elementsByPosition;
};

#endif // INDEXEDELEMENTLIST_H
