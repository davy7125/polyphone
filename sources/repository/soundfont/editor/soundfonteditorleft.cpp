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

#include "soundfonteditorleft.h"
#include "ui_soundfonteditorleft.h"
#include "contextmanager.h"
#include "soundfontinformation.h"
#include "repositorymanager.h"

SoundfontEditorLeft::SoundfontEditorLeft(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SoundfontEditorLeft)
{
    ui->setupUi(this);

    // Style
    ui->iconAuthor->setPixmap(ContextManager::theme()->getColoredSvg(":/icons/user.svg", QSize(16, 16), ThemeManager::ColorType::LIST_TEXT));
    ui->iconDate->setPixmap(ContextManager::theme()->getColoredSvg(":/icons/calendar.svg", QSize(16, 16), ThemeManager::ColorType::LIST_TEXT));
    ui->iconLicense->setPixmap(ContextManager::theme()->getColoredSvg(":/icons/copyright.svg", QSize(16, 16), ThemeManager::ColorType::LIST_TEXT));
    ui->iconWebsite->setPixmap(ContextManager::theme()->getColoredSvg(":/icons/globe.svg", QSize(16, 16), ThemeManager::ColorType::LIST_TEXT));
    ui->lineWebsite->resizeAfterChange(false);
    ui->lineWebsite->setColor(ContextManager::theme()->getColor(ThemeManager::ColorType::LIST_TEXT));

    // Possible licenses
    ui->comboLicense->addItem(trUtf8("Select a license..."), "");
    QMap<QString, QString> licenses = RepositoryManager::getInstance()->getLicenseLabels();
    foreach (QString key, licenses.keys())
        ui->comboLicense->addItem(licenses[key], key);

    // Possible categories
    ui->comboCategory->addItem(trUtf8("Select a category..."), -1);
    QList<int> categories = RepositoryManager::getInstance()->getCategories();
    foreach (int category, categories)
        ui->comboCategory->addItem(RepositoryManager::getInstance()->getCategoryName(category), category);

    // Possible properties
    ui->filterSampleSource->initialize(false, trUtf8("None"));
    QStringList properties = RepositoryManager::getInstance()->getProperties(SoundfontInformation::SAMPLE_SOURCE);
    foreach (QString property, properties)
        ui->filterSampleSource->addElement(property);

    ui->filterTimbre->initialize(false, trUtf8("None"));
    properties = RepositoryManager::getInstance()->getProperties(SoundfontInformation::TIMBRE);
    foreach (QString property, properties)
        ui->filterTimbre->addElement(property);

    ui->filterArticulation->initialize(false, trUtf8("None"));
    properties = RepositoryManager::getInstance()->getProperties(SoundfontInformation::ARTICULATION);
    foreach (QString property, properties)
        ui->filterArticulation->addElement(property);

    ui->filterGenre->initialize(false, trUtf8("None"));
    properties = RepositoryManager::getInstance()->getProperties(SoundfontInformation::GENRE);
    foreach (QString property, properties)
        ui->filterGenre->addElement(property);

    ui->filterMidiStandard->initialize(false, trUtf8("None"));
    properties = RepositoryManager::getInstance()->getProperties(SoundfontInformation::MIDI_STANDARD);
    foreach (QString property, properties)
        ui->filterMidiStandard->addElement(property);

    // Tags, with the possibility to create some
    ui->filterTag->setPossibleTags(RepositoryManager::getInstance()->getTags(), true);
}

SoundfontEditorLeft::~SoundfontEditorLeft()
{
    delete ui;
}

void SoundfontEditorLeft::initialize(SoundfontInformation *soundfontInfo)
{
    // Author
    ui->labelAuthor->setText(soundfontInfo->getAuthor());

    // Date
    ui->labelDate->setText(soundfontInfo->getDateTime().toString(Qt::SystemLocaleShortDate));

    // License
    ui->comboLicense->setCurrentText(RepositoryManager::getInstance()->getLicenseLabel(soundfontInfo->getLicense()));

    // Website
    ui->lineWebsite->setText(soundfontInfo->getWebsite());

    // Category
    ui->comboCategory->setCurrentText(RepositoryManager::getInstance()->getCategoryName(soundfontInfo->getCategoryId()));

    // Properties
    QMap<SoundfontInformation::Property, QStringList> properties = soundfontInfo->getProperties();
    ui->filterSampleSource->select(properties.contains(SoundfontInformation::SAMPLE_SOURCE) ?
                                       properties[SoundfontInformation::SAMPLE_SOURCE] : QStringList());
    ui->filterTimbre->select(properties.contains(SoundfontInformation::TIMBRE) ?
                                 properties[SoundfontInformation::TIMBRE] : QStringList());
    ui->filterArticulation->select(properties.contains(SoundfontInformation::ARTICULATION) ?
                                       properties[SoundfontInformation::ARTICULATION] : QStringList());
    ui->filterGenre->select(properties.contains(SoundfontInformation::GENRE) ?
                                properties[SoundfontInformation::GENRE] : QStringList());
    ui->filterMidiStandard->select(properties.contains(SoundfontInformation::MIDI_STANDARD) ?
                                       properties[SoundfontInformation::MIDI_STANDARD] : QStringList());

    // Tags
    ui->filterTag->select(soundfontInfo->getTags());
}
