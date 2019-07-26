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

#include "soundfontdownload.h"
#include "ui_soundfontdownload.h"
#include "soundfontdownloaddata.h"
#include "soundfontdownloadcell.h"

SoundfontDownload::SoundfontDownload(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SoundfontDownload)
{
    ui->setupUi(this);
}

SoundfontDownload::~SoundfontDownload()
{
    delete ui;
}

static bool sortFunction(const SoundfontDownloadData *s1, const SoundfontDownloadData *s2)
{
    return s1->getOrdering() < s2->getOrdering();
}

void SoundfontDownload::display(QList<SoundfontDownloadData *> data)
{
    // First order the list
    qSort(data.begin(), data.end(), sortFunction);

    // Add each download
    for (int i = 0; i < data.count(); i++)
    {
        SoundfontDownloadCell * cell = new SoundfontDownloadCell(this);
        cell->initialize(data[i]);
        ui->verticalLayout->addWidget(cell);
    }
}
