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

#include "filtertagcell.h"
#include "ui_filtertagcell.h"
#include "contextmanager.h"

FilterTagCell::FilterTagCell(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FilterTagCell)
{
    ui->setupUi(this);

    // Style of the widget
    QColor buttonBackground = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND);
    QColor buttonText = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT);
    QColor buttonHover = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND, ThemeManager::HOVERED);
    this->setStyleSheet("QPushButton,QLabel{background-color:" + buttonBackground.name() + "; color:" + buttonText.name() +
                        ";padding:5px;border:1px solid " + buttonBackground.name() + "}" +
                        "QPushButton:hover{ background-color:" + buttonHover.name() + ";border:1px solid " + buttonHover.name() + "}" +
                        "QLabel{border-top-left-radius:4px;border-bottom-left-radius:4px}" +
                        "QPushButton{border-top-right-radius:4px;border-bottom-right-radius:4px}");
    ui->pushDelete->setIcon(ContextManager::theme()->getColoredSvg(":/icons/close.svg", QSize(16, 16), ThemeManager::HIGHLIGHTED_TEXT));
}

FilterTagCell::~FilterTagCell()
{
    delete ui;
}

void FilterTagCell::on_pushDelete_clicked()
{
    emit(onRemoveClicked());
}

void FilterTagCell::setTagName(QString tagName)
{
    _tagName = tagName;
    ui->labelName->setTextToElide(tagName);
}

QString FilterTagCell::getTagName()
{
    return _tagName;
}
