/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2020 Davy Triponney                                **
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

#include "footersf2.h"
#include "ui_footersf2.h"
#include "soundfontmanager.h"

FooterSf2::FooterSf2(QWidget *parent) :
    AbstractFooter(parent),
    ui(new Ui::FooterSf2)
{
    ui->setupUi(this);
}

FooterSf2::~FooterSf2()
{
    delete ui;
}

void FooterSf2::updateInterface()
{
    if (_currentIds.empty())
    {
        ui->label_filename->setText("");
        return;
    }

    EltID id = _currentIds[0];
    id.typeElement = elementSf2;

    // Informations (file name and size)
    QString txt = SoundfontManager::getInstance()->getQstr(id, champ_filenameInitial);
    if (!txt.isEmpty())
    {
        QFile file(txt);
        if (file.open(QIODevice::ReadOnly))
        {
            int size = file.size();
            file.close();

            if (size > 1073741824)
            {
                // GB
                txt += QString(" (%1 %2)").arg((double)size / 1073741824, 3, 'f', 2).arg(tr("GB", "giga byte"));
            }
            else if (size > 1048576)
            {
                // MB
                txt += QString(" (%1 %2)").arg((double)size / 1048576, 3, 'f', 2).arg(tr("MB", "mega byte"));
            }
            else
            {
                // kB
                txt += QString(" (%1 %2)").arg((double)size / 1024, 3, 'f', 2).arg(tr("kB", "kilo byte"));
            }
        }
    }

    ui->label_filename->setTextToElide(txt);
}
