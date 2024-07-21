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

#ifndef SOUNDFONTCOMMENT_H
#define SOUNDFONTCOMMENT_H

#include <QWidget>
class QLabel;
class SoundfontCommentData;
class SoundfontCommentCell;

namespace Ui {
class SoundfontComment;
}

class SoundfontComment : public QWidget
{
    Q_OBJECT

public:
    explicit SoundfontComment(QWidget *parent = nullptr);
    ~SoundfontComment();

    void display(QList<SoundfontCommentData *> data, int level = 0);

private:
    void clear();

    Ui::SoundfontComment *ui;
    QList<SoundfontCommentCell * > _cells;
    QLabel * _label;
};

#endif // SOUNDFONTCOMMENT_H
