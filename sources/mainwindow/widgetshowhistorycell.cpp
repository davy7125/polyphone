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

#include "widgetshowhistorycell.h"
#include "ui_widgetshowhistorycell.h"
#include <QDateTime>
#include <QFile>
#include "contextmanager.h"

WidgetShowHistoryCell::Icons * WidgetShowHistoryCell::s_icons = nullptr;

WidgetShowHistoryCell::WidgetShowHistoryCell(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetShowHistoryCell),
    _isDir(false)
{
    ui->setupUi(this);

    // File icons
    if (s_icons == nullptr)
        s_icons = new Icons();

    // The style changes when the cell is activated
    _activeStyleSheet = "QLabel{color:" + ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT).name() + ";}";

    // Button for removing the file from the history, only visible when the cell is activated
    QColor removeHover = ThemeManager::mix(ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND),
                                           ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT), 0.2);
    ui->toolButtonRemove->setIcon(s_icons->_removeIcon);
    ui->toolButtonRemove->setStyleSheet("QToolButton{border:0;border-radius:2px;padding:2px;background-color:transparent}"
                                        "QToolButton:hover,QToolButton:pressed{background-color:" + removeHover.name() + "}");
    ui->toolButtonRemove->hide();
}

WidgetShowHistoryCell::Icons::Icons()
{
    _fileIcon = ContextManager::theme()->getColoredSvg(":/icons/file-audio.svg", QSize(48, 64), ThemeManager::LIST_TEXT);
    _fileIconActive = ContextManager::theme()->getColoredSvg(":/icons/file-audio.svg", QSize(48, 64), ThemeManager::HIGHLIGHTED_TEXT);
    _fileDirIcon = ContextManager::theme()->getColoredSvg(":/icons/folder.svg", QSize(48, 48), ThemeManager::LIST_TEXT);
    _fileDirIconActive = ContextManager::theme()->getColoredSvg(":/icons/folder.svg", QSize(48, 48), ThemeManager::HIGHLIGHTED_TEXT);
    _removeIcon = ContextManager::theme()->getColoredSvg(":/icons/trash.svg", QSize(16, 16), ThemeManager::HIGHLIGHTED_TEXT);
}

WidgetShowHistoryCell::~WidgetShowHistoryCell()
{
    delete ui;
}

void WidgetShowHistoryCell::setLink(QString filePath)
{
    _link = filePath;
    ui->labelLink->setTextToElide(filePath);
    _isDir = QFileInfo(_link).isDir();
    ui->iconFile->setMinimumHeight(_isDir ? 43 : 64);
    ui->iconFile->setMaximumHeight(_isDir ? 43 : 64);
    ui->iconFile->setPixmap(_isDir ? s_icons->_fileDirIcon : s_icons->_fileIcon);
}

QString WidgetShowHistoryCell::getLink()
{
    return _link;
}

void WidgetShowHistoryCell::setDateTime(QDateTime dateTime)
{
    if (dateTime.isValid())
        ui->labelDateTime->setText(dateTime.toString(QLocale().dateTimeFormat(QLocale::ShortFormat)));
    else
        ui->labelDateTime->setText("-");
}

void WidgetShowHistoryCell::setActive(bool isActive)
{
    if (isActive && this->styleSheet() == "")
    {
        this->setStyleSheet(_activeStyleSheet);
        ui->iconFile->setPixmap(_isDir ? s_icons->_fileDirIconActive : s_icons->_fileIconActive);
        ui->toolButtonRemove->show();
    }
    else if (!isActive && this->styleSheet() != "")
    {
        this->setStyleSheet("");
        ui->iconFile->setPixmap(_isDir ? s_icons->_fileDirIcon : s_icons->_fileIcon);
        ui->toolButtonRemove->hide();
    }
}

void WidgetShowHistoryCell::on_toolButtonRemove_clicked()
{
    emit removeClicked(_link);
}
