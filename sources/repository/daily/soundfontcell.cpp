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

#include "soundfontcell.h"
#include "ui_soundfontcell.h"
#include "soundfontinformation.h"
#include "soundfontfilter.h"
#include "contextmanager.h"
#include "repositorymanager.h"

SoundfontCell::IconContainer * SoundfontCell::s_icons = nullptr;

SoundfontCell::SoundfontCell(SoundfontInformation* soundfontInfo, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SoundfontCell),
    _active(true),
    _soundfontId(soundfontInfo->getId())
{
    ui->setupUi(this);
    this->installEventFilter(this);

    if (s_icons == NULL)
        s_icons = new IconContainer();

    // Title
    ui->labelTitle->setTextToElide(soundfontInfo->getTitle());

    // Author
    _authorTextNoColor = soundfontInfo->getAuthor();
    _authorTextNoColor = "<a style=\"text-decoration:none;color:%1;\" href=\"" + _authorTextNoColor + "\">" + _authorTextNoColor + "</a>";

    // Attributes
    connect(ui->line3, SIGNAL(itemClicked(SoundfontFilter*)), this, SIGNAL(itemClicked(SoundfontFilter*)));
    ui->line3->addCategory(soundfontInfo->getCategoryId());
    foreach (SoundfontInformation::Property key, soundfontInfo->getProperties().keys())
        foreach (QString value, soundfontInfo->getProperties()[key])
            ui->line3->addProperty(key, value);
    foreach (QString tag, soundfontInfo->getTags())
        ui->line3->addTag(tag);

    // Style when the cell is activated and when it's not
    QColor buttonBackground = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND);
    QColor buttonText = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT);
    QColor buttonBackgroundHover = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND, ThemeManager::HOVERED);
    QColor buttonBackgroundHover2 = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT, ThemeManager::HOVERED);
    _normalStyleSheet = QString("QPushButton{background-color:") + buttonBackground.name() + "; color:" + buttonText.name() +
            ";border:0px;padding:5px;border-radius:4px;}" +
            "QPushButton:hover{ background-color:" + buttonBackgroundHover.name() + ";}";
    _activeStyleSheet = QString("QPushButton{background-color:") + buttonText.name() + "; color:" + buttonBackground.name() +
            ";border:0px;padding:5px;border-radius:4px;}" +
            "QPushButton:hover{ background-color:" + buttonBackgroundHover2.name() + ";}" +
            "QLabel{color:" + ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT).name() + ";}";

    // Initialize the style
    this->setActive(false);
}

SoundfontCell::~SoundfontCell()
{
    delete ui;
}

void SoundfontCell::setActive(bool isActive)
{
    if (isActive && !_active)
    {
        this->setStyleSheet(_activeStyleSheet);
        ui->iconAuthor->setPixmap(s_icons->_userIconSelected);

        // Author texts
        QString linkColor = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT).name();
        ui->labelAuthor->setText(QString(_authorTextNoColor).arg(linkColor));
    }
    else if (!isActive && _active)
    {
        this->setStyleSheet(_normalStyleSheet);
        ui->iconAuthor->setPixmap(s_icons->_userIcon);

        // Author texts
        QString linkColor = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND).name();
        ui->labelAuthor->setText(QString(_authorTextNoColor).arg(linkColor));
    }
    _active = isActive;
}

SoundfontCell::IconContainer::IconContainer()
{
    // Base icons
    _userIcon = ContextManager::theme()->getColoredSvg(":/icons/user.svg", QSize(16, 16), ThemeManager::ColorType::LIST_TEXT);

    // Icons under selection
    _userIconSelected = ContextManager::theme()->getColoredSvg(":/icons/user.svg", QSize(16, 16), ThemeManager::ColorType::HIGHLIGHTED_TEXT);
}

void SoundfontCell::on_labelAuthor_linkActivated(const QString &link)
{
    SoundfontFilter * filter = new SoundfontFilter();
    filter->setSearchText(QString("Author:\"%1\"").arg(link));
    emit(itemClicked(filter));
}

int SoundfontCell::heightForWidth(int width) const
{
    return 30 + ui->line1->height() + ui->line2->height() + ui->line3->heightForWidth(width - 18);
}

bool SoundfontCell::hasHeightForWidth() const
{
    return true;
}

void SoundfontCell::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && _active)
    {
        RepositoryManager::getInstance()->openSoundfont(_soundfontId, true);
        event->accept();
        return;
    }

    QWidget::mouseDoubleClickEvent(event);
}
