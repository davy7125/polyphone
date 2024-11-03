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

#include "footeroverview.h"
#include "ui_footeroverview.h"
#include "soundfontmanager.h"

FooterOverview::FooterOverview(QWidget *parent) :
    AbstractFooter(parent),
    ui(new Ui::FooterOverview)
{
    ui->setupUi(this);
}

FooterOverview::~FooterOverview()
{
    delete ui;
}

void FooterOverview::updateInterface()
{
    // Number of rows
    if (_currentIds.empty() || !_currentIds.isElementUnique(elementSf2))
    {
        ui->labelInformation->setText("");
        return;
    }

    EltID id = _currentIds.getFirstId(elementSf2);
    switch (_currentIds[0].typeElement)
    {
    case elementRootSmpl:
        id.typeElement = elementSmpl;
        break;
    case elementRootInst:
        id.typeElement = elementInst;
        break;
    case elementRootPrst:
        id.typeElement = elementPrst;
        break;
    default:
        ui->labelInformation->setText("");
        return;
    }

    QList<int> indexes = SoundfontManager::getInstance()->getSiblings(id);
    ui->labelInformation->setText(tr("%n element(s)", "", indexes.count()));
}