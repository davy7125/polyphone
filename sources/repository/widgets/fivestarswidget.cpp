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

#include "fivestarswidget.h"
#include "ui_fivestarswidget.h"
#include "contextmanager.h"
#include <QPainter>

FiveStarsWidget::IconContainer * FiveStarsWidget::s_icons = nullptr;

FiveStarsWidget::FiveStarsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FiveStarsWidget),
    _isActive(true),
    _score(0)
{
    ui->setupUi(this);

    if (s_icons == NULL)
        s_icons = new IconContainer();

    // Star list
    _stars.append(ui->star1);
    _stars.append(ui->star2);
    _stars.append(ui->star3);
    _stars.append(ui->star4);
    _stars.append(ui->star5);
}

FiveStarsWidget::~FiveStarsWidget()
{
    delete ui;
}

void FiveStarsWidget::setScore(double score)
{
    _score = score;
    updateStars();
}

void FiveStarsWidget::setActive(bool isActive)
{
    if ((isActive && !_isActive) || (!isActive && _isActive))
    {
        _isActive = isActive;
        updateStars();
    }
}

void FiveStarsWidget::updateStars()
{
    int starNumber = _stars.count();
    for (int i = 0; i < starNumber; i++)
    {
        // Compute the percentage for the star number i
        double percentage = _score / 10.0 * starNumber - i;
        if (percentage < 0)
            percentage = 0.0;
        else if (percentage > 1)
            percentage = 1;

        // Update the image
        QPixmap mixedStar(s_icons->_baseStar);
        QPainter painter(&mixedStar);
        painter.drawPixmap(0, 0, mixedStar.width() * percentage, mixedStar.height(),
                           _isActive ? s_icons->_selectedColoredStar : s_icons->_coloredStar,
                           0, 0, mixedStar.width() * percentage, mixedStar.height());
        _stars[i]->setPixmap(mixedStar);
    }
}

FiveStarsWidget::IconContainer::IconContainer()
{
    _coloredStar = ContextManager::theme()->getColoredSvg(":/icons/star.svg", QSize(24, 24), ThemeManager::HIGHLIGHTED_BACKGROUND);
    _selectedColoredStar = ContextManager::theme()->getColoredSvg(":/icons/star.svg", QSize(24, 24), ThemeManager::HIGHLIGHTED_TEXT);
    _baseStar = ContextManager::theme()->getColoredSvg(":/icons/star.svg", QSize(24, 24), ThemeManager::LIST_TEXT);
}
