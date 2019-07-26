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

#include "soundfontviewerleft.h"
#include "ui_soundfontviewerleft.h"
#include "contextmanager.h"
#include "soundfontinformation.h"
#include "repositorymanager.h"

SoundfontViewerLeft::SoundfontViewerLeft(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SoundfontViewerLeft)
{
    ui->setupUi(this);

    // Style
    ui->iconAuthor->setPixmap(ContextManager::theme()->getColoredSvg(":/icons/user.svg", QSize(16, 16), ThemeManager::ColorType::LIST_TEXT));
    ui->iconDate->setPixmap(ContextManager::theme()->getColoredSvg(":/icons/calendar.svg", QSize(16, 16), ThemeManager::ColorType::LIST_TEXT));
    ui->iconLicense->setPixmap(ContextManager::theme()->getColoredSvg(":/icons/copyright.svg", QSize(16, 16), ThemeManager::ColorType::LIST_TEXT));
    ui->iconWebsite->setPixmap(ContextManager::theme()->getColoredSvg(":/icons/globe.svg", QSize(16, 16), ThemeManager::ColorType::LIST_TEXT));

    // Connection
    connect(ui->labelAuthor, SIGNAL(linkActivated(QString)), this, SLOT(onAuthorClicked(QString)));
}

SoundfontViewerLeft::~SoundfontViewerLeft()
{
    delete ui;
}

void SoundfontViewerLeft::initialize(SoundfontInformation *soundfontInfo)
{
    // Author
    QString author = soundfontInfo->getAuthor();
    ui->labelAuthor->setText("<a style=\"text-decoration:none;color:" +
                             ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND).name() +
                             "\" href=\"" + author + "\">" + author + "</a>");

    // Date
    ui->labelDate->setText(soundfontInfo->getDateTime().toString(Qt::SystemLocaleShortDate));

    // License
    ui->labelLicense->setText("<a style=\"text-decoration:none;color:" +
                              ContextManager::theme()->getColor(ThemeManager::WINDOW_TEXT).name() +
                              "\" href=\"" +
                              RepositoryManager::getInstance()->getLicenseLink(soundfontInfo->getLicense()) +
                              "\">" + RepositoryManager::getInstance()->getLicenseLabel(soundfontInfo->getLicense()) + "</a>");

    // Website
    if (soundfontInfo->getWebsite() == "")
    {
        ui->labelWebsite->hide();
        ui->iconWebsite->hide();
    }
    else
    {
        ui->labelWebsite->show();
        ui->iconWebsite->show();
        ui->labelWebsite->setText("<a href='" + soundfontInfo->getWebsite() + "' style='text-decoration:none;color:" +
                                  ContextManager::theme()->getColor(ThemeManager::WINDOW_TEXT).name()+
                                  "'>" + soundfontInfo->getWebsite() + "</a>");
    }

    // Category
    connect(ui->attributeCategory, SIGNAL(itemClicked(SoundfontFilter*)), this, SIGNAL(itemClicked(SoundfontFilter*)));
    ui->attributeCategory->addCategory(soundfontInfo->getCategoryId());

    // Properties
    SoundfontInformation::Property property = SoundfontInformation::SAMPLE_SOURCE;
    if (soundfontInfo->getProperties().contains(property) && !soundfontInfo->getProperties()[property].empty())
    {
        connect(ui->attributeSampleSource, SIGNAL(itemClicked(SoundfontFilter*)), this, SIGNAL(itemClicked(SoundfontFilter*)));
        foreach (QString value, soundfontInfo->getProperties()[property])
            ui->attributeSampleSource->addProperty(property, value);
    }
    else
    {
        ui->attributeSampleSource->hide();
        ui->labelSampleSource->hide();
    }

    property = SoundfontInformation::TIMBRE;
    if (soundfontInfo->getProperties().contains(property) && !soundfontInfo->getProperties()[property].empty())
    {
        connect(ui->attributeTimbre, SIGNAL(itemClicked(SoundfontFilter*)), this, SIGNAL(itemClicked(SoundfontFilter*)));
        foreach (QString value, soundfontInfo->getProperties()[property])
            ui->attributeTimbre->addProperty(property, value);
    }
    else
    {
        ui->attributeTimbre->hide();
        ui->labelTimbre->hide();
    }

    property = SoundfontInformation::ARTICULATION;
    if (soundfontInfo->getProperties().contains(property) && !soundfontInfo->getProperties()[property].empty())
    {
        connect(ui->attributeArticulation, SIGNAL(itemClicked(SoundfontFilter*)), this, SIGNAL(itemClicked(SoundfontFilter*)));
        foreach (QString value, soundfontInfo->getProperties()[property])
            ui->attributeArticulation->addProperty(property, value);
    }
    else
    {
        ui->attributeArticulation->hide();
        ui->labelArticulation->hide();
    }

    property = SoundfontInformation::GENRE;
    if (soundfontInfo->getProperties().contains(property) && !soundfontInfo->getProperties()[property].empty())
    {
        connect(ui->attributeGenre, SIGNAL(itemClicked(SoundfontFilter*)), this, SIGNAL(itemClicked(SoundfontFilter*)));
        foreach (QString value, soundfontInfo->getProperties()[property])
            ui->attributeGenre->addProperty(property, value);
    }
    else
    {
        ui->attributeGenre->hide();
        ui->labelGenre->hide();
    }

    property = SoundfontInformation::MIDI_STANDARD;
    if (soundfontInfo->getProperties().contains(property) && !soundfontInfo->getProperties()[property].empty())
    {
        connect(ui->attributeMidiStandard, SIGNAL(itemClicked(SoundfontFilter*)), this, SIGNAL(itemClicked(SoundfontFilter*)));
        foreach (QString value, soundfontInfo->getProperties()[property])
            ui->attributeMidiStandard->addProperty(property, value);
    }
    else
    {
        ui->attributeMidiStandard->hide();
        ui->labelMidiStandard->hide();
    }

    // Tags
    if (soundfontInfo->getTags().isEmpty())
    {
        ui->labelTags->hide();
        ui->attributeTag->hide();
    }
    else
    {
        connect(ui->attributeTag, SIGNAL(itemClicked(SoundfontFilter*)), this, SIGNAL(itemClicked(SoundfontFilter*)));
        foreach (QString tag, soundfontInfo->getTags())
            ui->attributeTag->addTag(tag);
    }
}

void SoundfontViewerLeft::onAuthorClicked(const QString &link)
{
    SoundfontFilter * filter = new SoundfontFilter();
    filter->setSearchText(QString("Author:\"%1\"").arg(link));
    emit(itemClicked(filter));
}
