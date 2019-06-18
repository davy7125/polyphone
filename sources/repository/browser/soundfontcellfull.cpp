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

#include "soundfontcellfull.h"
#include "ui_soundfontcellfull.h"
#include "soundfontinformation.h"
#include "repositorymanager.h"
#include "contextmanager.h"

SoundfontCellFull::IconContainer * SoundfontCellFull::s_icons = nullptr;

SoundfontCellFull::SoundfontCellFull(SoundfontInformation* soundfontInfo, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SoundfontCellFull),
    _active(true),
    _soundfontId(soundfontInfo->getId())
{
    ui->setupUi(this);

    if (s_icons == nullptr)
        s_icons = new IconContainer();

    // Title
    ui->labelTitle->setTextToElide(soundfontInfo->getTitle());

    // Numbers
    ui->labelCommentNumber->setText(QString::number(soundfontInfo->getCommentNumber()));
    ui->labelDownloadNumber->setText(QString::number(soundfontInfo->getDownloadNumber()));
    ui->widgetStars->setScore(soundfontInfo->getRating());

    // Author
    _authorTextNoColor = soundfontInfo->getAuthor();
    _authorTextNoColor = "<a style=\"text-decoration:none;color:%1;\" href=\"" + _authorTextNoColor + "\">" + _authorTextNoColor + "</a>";

    // Date
    ui->labelDate->setText(soundfontInfo->getDateTime().toString(Qt::SystemLocaleShortDate));

    // License
    ui->labelLicense->setTextToElide(RepositoryManager::getInstance()->getLicenseLabel(soundfontInfo->getLicense()),
                                     RepositoryManager::getInstance()->getLicenseLink(soundfontInfo->getLicense()));

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
    QString tmp = QString("QPushButton{background-color:%1; color:%2;border:0px;padding:5px;border-radius:4px;}") +
            "QPushButton:hover{ background-color:%3;}QLabel#labelAuthor,ElidedLabel#labelLicense{color:%1;}";
    _normalStyleSheet = tmp.arg(buttonBackground.name()).arg(buttonText.name()).arg(buttonBackgroundHover.name());
    _activeStyleSheet = tmp.arg(buttonText.name()).arg(buttonBackground.name()).arg(buttonBackgroundHover2.name()) +
            "QLabel{color:" + ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT).name() + ";}";

    // Initialize the style
    this->setActive(false);
}

SoundfontCellFull::~SoundfontCellFull()
{
    delete ui;
}

void SoundfontCellFull::setActive(bool isActive)
{
    if (isActive && !_active)
    {
        // Colors
        this->setStyleSheet(_activeStyleSheet);

        // Icons
        ui->iconComment->setPixmap(s_icons->_commentIconSelected);
        ui->iconDownload->setPixmap(s_icons->_downloadIconSelected);
        ui->iconAuthor->setPixmap(s_icons->_userIconSelected);
        ui->iconDate->setPixmap(s_icons->_dateIconSelected);
        ui->iconLicense->setPixmap(s_icons->_copyrightIconSelected);

        // Author and license texts
        QColor linkColor = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT);
        ui->labelAuthor->setText(QString(_authorTextNoColor).arg(linkColor.name()));
    }
    else if (!isActive && _active)
    {
        // Colors
        this->setStyleSheet(_normalStyleSheet);

        // Icons
        ui->iconComment->setPixmap(s_icons->_commentIcon);
        ui->iconDownload->setPixmap(s_icons->_downloadIcon);
        ui->iconAuthor->setPixmap(s_icons->_userIcon);
        ui->iconDate->setPixmap(s_icons->_dateIcon);
        ui->iconLicense->setPixmap(s_icons->_copyrightIcon);

        // Author and license texts
        QColor linkColor = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND);
        ui->labelAuthor->setText(QString(_authorTextNoColor).arg(linkColor.name()));
    }
    _active = isActive;

    // Stars
    ui->widgetStars->setActive(isActive);
}

SoundfontCellFull::IconContainer::IconContainer()
{
    // Base icons
    _commentIcon = ContextManager::theme()->getColoredSvg(":/icons/comment.svg", QSize(24, 24), ThemeManager::ColorType::LIST_TEXT);
    _downloadIcon = ContextManager::theme()->getColoredSvg(":/icons/download.svg", QSize(24, 24), ThemeManager::ColorType::LIST_TEXT);
    _userIcon = ContextManager::theme()->getColoredSvg(":/icons/user.svg", QSize(16, 16), ThemeManager::ColorType::LIST_TEXT);
    _dateIcon = ContextManager::theme()->getColoredSvg(":/icons/calendar.svg", QSize(16, 16), ThemeManager::ColorType::LIST_TEXT);
    _copyrightIcon = ContextManager::theme()->getColoredSvg(":/icons/copyright.svg", QSize(16, 16), ThemeManager::ColorType::LIST_TEXT);

    // Icons under selection
    _commentIconSelected = ContextManager::theme()->getColoredSvg(":/icons/comment.svg", QSize(24, 24), ThemeManager::ColorType::HIGHLIGHTED_TEXT);
    _downloadIconSelected = ContextManager::theme()->getColoredSvg(":/icons/download.svg", QSize(24, 24), ThemeManager::ColorType::HIGHLIGHTED_TEXT);
    _userIconSelected = ContextManager::theme()->getColoredSvg(":/icons/user.svg", QSize(16, 16), ThemeManager::ColorType::HIGHLIGHTED_TEXT);
    _dateIconSelected = ContextManager::theme()->getColoredSvg(":/icons/calendar.svg", QSize(16, 16), ThemeManager::ColorType::HIGHLIGHTED_TEXT);
    _copyrightIconSelected = ContextManager::theme()->getColoredSvg(":/icons/copyright.svg", QSize(16, 16), ThemeManager::ColorType::HIGHLIGHTED_TEXT);
}

void SoundfontCellFull::on_labelAuthor_linkActivated(const QString &link)
{
    SoundfontFilter * filter = new SoundfontFilter();
    filter->setSearchText(QString("Author:\"%1\"").arg(link));
    emit(itemClicked(filter));
}

int SoundfontCellFull::heightForWidth(int width) const
{
    return 30 + ui->line1->height() + ui->line2->height() + ui->line3->heightForWidth(width - 18);
}

bool SoundfontCellFull::hasHeightForWidth() const
{
    return true;
}

void SoundfontCellFull::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && _active)
    {
        event->accept();
        RepositoryManager::getInstance()->openSoundfont(_soundfontId, false);
        return;
    }

    QWidget::mouseDoubleClickEvent(event);
}
