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

#include "widgetshowhistorycell.h"
#include "ui_widgetshowhistorycell.h"
#include <QDateTime>
#include <QFile>
#include "contextmanager.h"

WidgetShowHistoryCell::Icons * WidgetShowHistoryCell::s_icons = nullptr;

WidgetShowHistoryCell::WidgetShowHistoryCell(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetShowHistoryCell)
{
    ui->setupUi(this);

    // File icons
    if (s_icons == NULL)
        s_icons = new Icons();

    ui->iconFile->setPixmap(s_icons->_fileIcon);

    // The style changes when the cell is activated
    _activeStyleSheet = "QLabel{color:" + ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT).name() + ";}";
}

WidgetShowHistoryCell::Icons::Icons()
{
    _fileIcon = ContextManager::theme()->getColoredSvg(":/icons/file-audio.svg", QSize(48, 64), ThemeManager::LIST_TEXT);
    _fileIconActive = ContextManager::theme()->getColoredSvg(":/icons/file-audio.svg", QSize(48, 64), ThemeManager::HIGHLIGHTED_TEXT);
}

WidgetShowHistoryCell::~WidgetShowHistoryCell()
{
    delete ui;
}

void WidgetShowHistoryCell::setLink(QString filePath)
{
    _link = filePath;
    ui->labelLink->setText(filePath);
}

QString WidgetShowHistoryCell::getLink()
{
    return _link;
}

void WidgetShowHistoryCell::setDateTime(QDateTime dateTime)
{
    if (dateTime.isValid())
        ui->labelDateTime->setText(dateTime.toString("yyyy/MM/dd hh:mm"));
    else
        ui->labelDateTime->setText("-");
}

void WidgetShowHistoryCell::setActive(bool isActive)
{
    if (isActive && this->styleSheet() == "")
    {
        this->setStyleSheet(_activeStyleSheet);
        ui->iconFile->setPixmap(s_icons->_fileIconActive);
    }
    else if (!isActive && this->styleSheet() != "")
    {
        this->setStyleSheet("");
        ui->iconFile->setPixmap(s_icons->_fileIcon);
    }
}
