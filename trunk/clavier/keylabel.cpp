/*
    Virtual Piano Widget for Qt4
    Copyright (C) 2008-2014, Pedro Lopez-Cabanillas <plcl@users.sf.net>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; If not, see <http://www.gnu.org/licenses/>.
*/

#include "keylabel.h"
#include "pianokey.h"
#include <QFont>

KeyLabel::KeyLabel(QGraphicsItem *parent) : QGraphicsTextItem(parent)
{
    setAcceptedMouseButtons(Qt::NoButton);
    //rotate(270);
    setRotation(rotation() + 270);
}

QRectF KeyLabel::boundingRect() const
{
    PianoKey* key = static_cast<PianoKey*>(parentItem());
    return mapRectFromScene(key->rect());
}
