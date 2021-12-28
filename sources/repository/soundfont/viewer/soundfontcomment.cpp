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

#include "soundfontcomment.h"
#include "ui_soundfontcomment.h"
#include "soundfontcommentdata.h"
#include "soundfontcommentcell.h"
#include <QLabel>

SoundfontComment::SoundfontComment(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SoundfontComment),
    _label(nullptr)
{
    ui->setupUi(this);
}

SoundfontComment::~SoundfontComment()
{
    this->clear();
    delete ui;
}

bool sortRecentFirst(const SoundfontCommentData *s1, const SoundfontCommentData *s2)
{
    return s1->getCreationTime() > s2->getCreationTime();
}

bool sortLastFirst(const SoundfontCommentData *s1, const SoundfontCommentData *s2)
{
    return s1->getCreationTime() < s2->getCreationTime();
}

void SoundfontComment::clear()
{
    while (!_cells.isEmpty())
        delete _cells.takeFirst();
    delete _label;
    _label = nullptr;
}

void SoundfontComment::display(QList<SoundfontCommentData *> data, int level)
{
    // First clear everything
    this->clear();

    // Order the list
    if (level == 0)
        std::sort(data.begin(), data.end(), sortRecentFirst);
    else
        std::sort(data.begin(), data.end(), sortLastFirst);

    // Add each comment
    for (int i = 0; i < data.count(); i++)
    {
        SoundfontCommentCell * cell = new SoundfontCommentCell(this);
        cell->initialize(data[i], level);
        ui->verticalLayout->addWidget(cell);
        _cells << cell;

        // Display the children
        this->display(data[i]->getChildren(), level + 1);
    }

    if (level == 0 && data.isEmpty())
    {
        _label = new QLabel(this);
        _label->setText(tr("No comments."));
        ui->verticalLayout->addWidget(_label);
    }
}
