/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013 Davy Triponney                                     **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: http://www.polyphone.fr/                             **
**             Date: 01.01.2013                                           **
***************************************************************************/

#ifndef KEYMAPPER_H
#define KEYMAPPER_H

#include <QVector>
#include <QList>
#include <QKeySequence>

class KeyMapper
{
public:
    KeyMapper();
    void addCombinaisonKey(int key, QKeySequence combinaison);
    void setCombinaisonKey(int key, int numCombinaison, QString sequence);
    void setOctave(int numOctave);
    int getKey(QKeySequence combinaison);
    QString getSequence(int key, int numCombinaison);
    int getRootKey()    { return ROOT_KEY; }
    int getKeyCount()   { return KEY_NUMBER; }

private:
    QVector<QList<QKeySequence> > _listCombinaisons;
    int _octave;
    static int KEY_NUMBER;
    static int ROOT_KEY;
};

#endif // KEYMAPPER_H
