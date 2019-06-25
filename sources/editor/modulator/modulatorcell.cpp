/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2019 Davy Triponney                                **
**                2014      Andrea Celani                                 **
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

#include "modulatorcell.h"
#include "ui_modulatorcell.h"
#include <QPainter>
#include "contextmanager.h"
#include "soundfontmanager.h"

ModulatorCell::ModulatorCell(EltID id, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ModulatorCell),
    _id(id),
    _sm(SoundfontManager::getInstance())
{
    ui->setupUi(this);

    // Colors
    _labelColor = ThemeManager::mix(
                ContextManager::theme()->getColor(ThemeManager::LIST_TEXT),
                ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND),
                0.5).name();
    _computationAreaColor = ThemeManager::mix(
                ContextManager::theme()->getColor(ThemeManager::LIST_BACKGROUND),
                ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND),
                0.25);

    // Label style
    ui->labelCoeff->setStyleSheet("QLabel{color:" + _labelColor.name() + "}");
    ui->labelTransform->setStyleSheet("QLabel{color:" + _labelColor.name() + "}");

    // Current number
    int modCount = 0;
    foreach (int i, SoundfontManager::getInstance()->getSiblings(id))
    {
        if (i == _id.indexMod)
        {
            ui->labelModNumber->setText(trUtf8("Modulator") + "\n#" + QString::number(modCount + 1));
            break;
        }
        modCount++;
    }

    // Populate comboboxes
    ui->comboSource1->initialize(id, true);
    ui->comboSource2->initialize(id, false);
    ui->comboDestination->initialize(id);

    // Shapes
    ui->widgetShape1->initialize(id, true);
    ui->widgetShape2->initialize(id, false);

    // Coefficient
    ui->spinAmount->setValue(SoundfontManager::getInstance()->get(id, champ_modAmount).shValue);

    // Transform
    ui->comboTransform->setCurrentIndex(SoundfontManager::getInstance()->get(id, champ_sfModTransOper).wValue == 2 ? 1 : 0);
}

ModulatorCell::~ModulatorCell()
{
    delete ui;
}

void ModulatorCell::paintEvent(QPaintEvent* event)
{
    QPainter p(this);

    // Highlight the computation area
    int x1 = ui->widgetShape1->x() - 6;
    int x2 = ui->comboTransform->x() + ui->comboTransform->width() + 6;
    int y1 = ui->widgetShape1->y() - 6;
    int y2 = ui->widgetShape2->y() + ui->widgetShape2->height() + 6;
    p.fillRect(QRect(x1, y1, x2 - x1, y2 - y1), _computationAreaColor);

    // Draw lines
    p.setPen(QPen(_labelColor, 3));
    p.drawLine(ui->comboSource1->x() + ui->comboSource1->width(), ui->comboSource1->y() + ui->comboSource1->height() / 2,
               ui->widgetShape1->x() + ui->widgetShape1->width() + 34, ui->comboSource1->y() + ui->comboSource1->height() / 2);
    p.drawLine(ui->comboSource2->x() + ui->comboSource2->width(), ui->comboSource2->y() + ui->comboSource2->height() / 2,
               ui->comboDestination->x(), ui->comboSource2->y() + ui->comboSource2->height() / 2);
    p.drawLine(ui->widgetShape1->x() + ui->widgetShape1->width() + 34, ui->comboSource1->y() + ui->comboSource1->height() / 2,
               ui->widgetShape1->x() + ui->widgetShape1->width() + 34, ui->comboSource2->y() + ui->comboSource2->height() / 2);

    // Draw a multiplication sign
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::HighQualityAntialiasing);
    p.setBrush(_labelColor);
    p.drawEllipse(ui->widgetShape1->x() + ui->widgetShape1->width() + 34 - 8, ui->comboSource2->y() + ui->comboSource2->height() / 2 - 8, 16, 16);
    p.setPen(QPen(_computationAreaColor, 2));
    p.drawLine(ui->widgetShape1->x() + ui->widgetShape1->width() + 34 - 3, ui->comboSource2->y() + ui->comboSource2->height() / 2 - 3,
               ui->widgetShape1->x() + ui->widgetShape1->width() + 34 + 3, ui->comboSource2->y() + ui->comboSource2->height() / 2 + 3);
    p.drawLine(ui->widgetShape1->x() + ui->widgetShape1->width() + 34 - 3, ui->comboSource2->y() + ui->comboSource2->height() / 2 + 3,
               ui->widgetShape1->x() + ui->widgetShape1->width() + 34 + 3, ui->comboSource2->y() + ui->comboSource2->height() / 2 - 3);

    QWidget::paintEvent(event);
}

AttributeType ModulatorCell::getTargetAttribute()
{
    return ui->comboDestination->getCurrentAttribute();
}
