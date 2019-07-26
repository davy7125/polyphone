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

#include "soundfontbrowser.h"
#include "ui_soundfontbrowser.h"
#include "contextmanager.h"
#include "soundfontfilter.h"
#include "repositorymanager.h"
#include "soundfontcellfull.h"
#include "utils.h"
#include <QResizeEvent>
#include <QDesktopServices>

SoundfontBrowser::SoundfontBrowser(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SoundfontBrowser),
    _loadingFilter(false),
    _lastWidth(-1)
{
    ui->setupUi(this);

    // Style
    ui->pushRetry->setIcon(ContextManager::theme()->getColoredSvg(":/icons/reload.svg", QSize(16, 16), ThemeManager::HIGHLIGHTED_TEXT));
    ui->pushRetry->setStyleSheet("QPushButton{background-color:" +
                                 ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND).name() +
                                 ";border-radius:5px;padding:5px}");
    ui->widgetColored->setStyleSheet("QWidget{background-color:" + ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND).name() + "}");
    ui->widgetColored2->setStyleSheet("QWidget{background-color:" + ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND).name() + "}");
    QString titleStyleSheet = "QFrame{background-color:" + ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND).name() +
            ";color:" + ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT).name() +
            "}";
    ui->frameTitle->setStyleSheet(titleStyleSheet);
    ui->frameSearch->setStyleSheet(titleStyleSheet);
    ui->listWidget->setStyleSheet("QListWidget{border:1px solid " +
                                  this->palette().dark().color().name() +
                                  ";border-top:0;border-right:0;border-bottom:0}");
    ui->pushBecomePremium->setStyleSheet("QPushButton{border:1px solid " +
                                         this->palette().dark().color().name() +
                                         ";border-top:0;border-right:0;padding:4px;"
                                         "color:" + ContextManager::theme()->getFixedColor(ThemeManager::RED, ThemeManager::BUTTON_BACKGROUND).name() + "}");
    ui->comboSort->view()->window()->setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    ui->comboSort->setStyleSheet(QString("QComboBox,QComboBox::drop-down{border-top-right-radius: 2px;border-bottom-right-radius: 2px}") +
                                 "QComboBox::drop-down{border-width:0}" +
                                 "QComboBox{padding: 0;}");
    ui->labelSort->setPixmap(ContextManager::theme()->getColoredSvg(":/icons/sort.svg", QSize(16, 16), ThemeManager::HIGHLIGHTED_BACKGROUND));
    ui->labelSort->setStyleSheet("QLabel{border-top-left-radius:2px;border-bottom-left-radius:2px;padding: 0 5px;background-color:" +
                                 ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT).name() + "}");
    QString resetHoverColor = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT, ThemeManager::HOVERED).name();

    ui->pushResetFilters->setStyleSheet("QPushButton{background-color:" + ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT).name() +
                                        ";color:" + ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND).name() +
                                        ";border-radius:2px;border:0;padding:2px 5px}" +
                                        "QPushButton:hover{background-color:" + resetHoverColor + "}");
    ui->lineSearch->setStyleSheet("QLineEdit{border:0;border-top-left-radius:2px;border-bottom-left-radius:2px}");
    ui->pushClearSearch->setIcon(ContextManager::theme()->getColoredSvg(":/icons/close.svg", QSize(16, 16), ThemeManager::HIGHLIGHTED_BACKGROUND));
    ui->pushClearSearch->setStyleSheet("QPushButton{border:0;border-top-right-radius:2px;border-bottom-right-radius:2px;background-color:" +
                                       ContextManager::theme()->getColor(ThemeManager::LIST_BACKGROUND).name() + "}");
    QColor color = ThemeManager::mix(
                ContextManager::theme()->getColor(ThemeManager::LIST_BACKGROUND),
                ContextManager::theme()->getColor(ThemeManager::LIST_TEXT),
                0.5);
    ui->labelNoResult->setStyleSheet("QLabel{color:" + color.name() +
                                     ";border:1px solid " + this->palette().dark().color().name() +
                                     ";border-top:0;border-right:0;border-bottom:0}");

    // Connection with the repository manager
    RepositoryManager * rm = RepositoryManager::getInstance();
    connect(rm, SIGNAL(initializing()), this, SLOT(initialize()));
    connect(rm, SIGNAL(ready(QString)), this, SLOT(soundfontListAvailable(QString)));

    // Filter connection
    connect(ui->lineSearch, SIGNAL(textChanged(QString)), this, SLOT(updateList()));
    connect(ui->filterCategory, SIGNAL(selectionChanged()), this, SLOT(updateList()));
    connect(ui->filterLicense, SIGNAL(selectionChanged()), this, SLOT(updateList()));
    connect(ui->filterSampleSource, SIGNAL(selectionChanged()), this, SLOT(updateList()));
    connect(ui->filterTimbre, SIGNAL(selectionChanged()), this, SLOT(updateList()));
    connect(ui->filterArticulation, SIGNAL(selectionChanged()), this, SLOT(updateList()));
    connect(ui->filterGenre, SIGNAL(selectionChanged()), this, SLOT(updateList()));
    connect(ui->filterMidiStandard, SIGNAL(selectionChanged()), this, SLOT(updateList()));
    connect(ui->filterTag, SIGNAL(selectionChanged()), this, SLOT(updateList()));

    // Connection with the user manager
    connect(UserManager::getInstance(), SIGNAL(connectionStateChanged(UserManager::ConnectionState)),
            this, SLOT(userStatusChanged(UserManager::ConnectionState)));
}

SoundfontBrowser::~SoundfontBrowser()
{
    delete ui;
}

void SoundfontBrowser::initialize()
{
    ui->spinner->startAnimation();
    ui->stackedWidget->setCurrentIndex(0);
}

void SoundfontBrowser::soundfontListAvailable(QString error)
{
    if (error.isEmpty())
    {
        fillFilter();
        applyFilter(nullptr);
        ui->stackedWidget->setCurrentIndex(1);
    }
    else
    {
        ui->stackedWidget->setCurrentIndex(2);
    }
}

void SoundfontBrowser::fillFilter()
{
    // Categories
    ui->filterCategory->initialize(true);
    QList<int> categories = RepositoryManager::getInstance()->getCategories();
    foreach (int category, categories)
        ui->filterCategory->addElement(RepositoryManager::getInstance()->getCategoryName(category), category);

    // License
    ui->filterLicense->initialize();
    ui->filterLicense->addElement(trUtf8("commercial use"), static_cast<int>(SoundfontInformation::LicenseFlag::COMMERCIAL_USE));
    ui->filterLicense->addElement(trUtf8("share after editing"), static_cast<int>(SoundfontInformation::LicenseFlag::SHARE_MODIFIED));

    // Properties
    ui->filterSampleSource->initialize();
    QStringList properties = RepositoryManager::getInstance()->getProperties(SoundfontInformation::Property::SAMPLE_SOURCE);
    foreach (QString property, properties)
        ui->filterSampleSource->addElement(property);

    ui->filterTimbre->initialize();
    properties = RepositoryManager::getInstance()->getProperties(SoundfontInformation::Property::TIMBRE);
    foreach (QString property, properties)
        ui->filterTimbre->addElement(property);

    ui->filterArticulation->initialize();
    properties = RepositoryManager::getInstance()->getProperties(SoundfontInformation::Property::ARTICULATION);
    foreach (QString property, properties)
        ui->filterArticulation->addElement(property);

    ui->filterGenre->initialize();
    properties = RepositoryManager::getInstance()->getProperties(SoundfontInformation::Property::GENRE);
    foreach (QString property, properties)
        ui->filterGenre->addElement(property);

    ui->filterMidiStandard->initialize();
    properties = RepositoryManager::getInstance()->getProperties(SoundfontInformation::Property::MIDI_STANDARD);
    foreach (QString property, properties)
        ui->filterMidiStandard->addElement(property);

    // Tags
    ui->filterTag->setPossibleTags(RepositoryManager::getInstance()->getTags());
}

void SoundfontBrowser::applyFilter(SoundfontFilter * filter)
{
    _loadingFilter = true;
    if (filter)
    {
        // Text
        ui->lineSearch->setText(filter->searchText());

        // Category
        ui->filterCategory->select(filter->category());

        // License
        QList<int> selectedLicenses;
        if (filter->licenseFlags().testFlag(SoundfontInformation::LicenseFlag::COMMERCIAL_USE))
            selectedLicenses.append(static_cast<int>(SoundfontInformation::LicenseFlag::COMMERCIAL_USE));
        if (filter->licenseFlags().testFlag(SoundfontInformation::LicenseFlag::SHARE_MODIFIED))
            selectedLicenses.append(static_cast<int>(SoundfontInformation::LicenseFlag::SHARE_MODIFIED));
        ui->filterLicense->select(selectedLicenses);

        // Properties
        ui->filterSampleSource->select(filter->properties(SoundfontInformation::Property::SAMPLE_SOURCE));
        ui->filterTimbre->select(filter->properties(SoundfontInformation::Property::TIMBRE));
        ui->filterArticulation->select(filter->properties(SoundfontInformation::Property::ARTICULATION));
        ui->filterGenre->select(filter->properties(SoundfontInformation::Property::GENRE));
        ui->filterMidiStandard->select(filter->properties(SoundfontInformation::Property::MIDI_STANDARD));

        // Tags
        ui->filterTag->select(filter->tags());
    }
    else
    {
        // Restore all filters
        ui->filterCategory->select(-1);
        ui->filterLicense->select(-1);
        ui->filterSampleSource->select(QStringList());
        ui->filterTimbre->select(QStringList());
        ui->filterArticulation->select(QStringList());
        ui->filterGenre->select(QStringList());
        ui->filterMidiStandard->select(QStringList());
        ui->filterTag->select(QStringList());
    }
    _loadingFilter = false;

    // Update the soundfont list
    updateList();
}

void SoundfontBrowser::on_pushRetry_clicked()
{
    RepositoryManager::getInstance()->initialize();
}

void SoundfontBrowser::on_pushResetFilters_clicked()
{
    applyFilter(nullptr);
}

SoundfontFilter * SoundfontBrowser::getFilter()
{
    SoundfontFilter * filter = new SoundfontFilter();

    // Textual search
    filter->setSearchText(ui->lineSearch->text());

    // Category id
    filter->setCategory(ui->filterCategory->getSelectedId());

    // License
    int sum = 0;
    QList<int> licenseIds = ui->filterLicense->getSelectedIds();
    foreach (int licenseId, licenseIds)
        sum += licenseId;
    filter->setLicenseFlags(static_cast<SoundfontInformation::LicenseFlags>(sum));

    // Properties
    filter->setProperties(SoundfontInformation::Property::SAMPLE_SOURCE, ui->filterSampleSource->getSelectedElements());
    filter->setProperties(SoundfontInformation::Property::TIMBRE, ui->filterTimbre->getSelectedElements());
    filter->setProperties(SoundfontInformation::Property::ARTICULATION, ui->filterArticulation->getSelectedElements());
    filter->setProperties(SoundfontInformation::Property::GENRE, ui->filterGenre->getSelectedElements());
    filter->setProperties(SoundfontInformation::Property::MIDI_STANDARD, ui->filterMidiStandard->getSelectedElements());

    // Tags
    filter->setTags(ui->filterTag->getSelection());

    return filter;
}

void SoundfontBrowser::updateList()
{
    if (_loadingFilter)
        return;

    // Build a filter
    SoundfontFilter * filter = this->getFilter();

    // Filter soundfonts
    _currentSoundfontInfos = RepositoryManager::getInstance()->getSoundfontInformation(filter);
    delete filter;

    updateList2();
}

bool sortByDate(SoundfontInformation * si1, SoundfontInformation * si2)
{
    return si1->getDateTime() > si2->getDateTime();
}

bool sortByDownloads(SoundfontInformation * si1, SoundfontInformation * si2)
{
    return si1->getDownloadNumber() > si2->getDownloadNumber();
}

bool sortByScore(SoundfontInformation * si1, SoundfontInformation * si2)
{
    return si1->getRating() > si2->getRating();
}

bool sortByTitle(SoundfontInformation * si1, SoundfontInformation * si2)
{
    return Utils::naturalOrder(si1->getTitle().toLower(), si2->getTitle().toLower()) < 0;
}

void SoundfontBrowser::updateList2()
{
    // Sort
    switch (ui->comboSort->currentIndex())
    {
    case 0:
        qSort(_currentSoundfontInfos.begin(), _currentSoundfontInfos.end(), sortByDate);
        break;
    case 1:
        qSort(_currentSoundfontInfos.begin(), _currentSoundfontInfos.end(), sortByDownloads);
        break;
    case 2:
        qSort(_currentSoundfontInfos.begin(), _currentSoundfontInfos.end(), sortByScore);
        break;
    case 3:
        qSort(_currentSoundfontInfos.begin(), _currentSoundfontInfos.end(), sortByTitle);
        break;
    }

    // Fill with the soundfonts
    ui->listWidget->clear();
    foreach (SoundfontInformation * si, _currentSoundfontInfos)
    {
        SoundfontCellFull * cell = new SoundfontCellFull(si, this);
        connect(cell, SIGNAL(itemClicked(SoundfontFilter*)), this, SLOT(applyFilter(SoundfontFilter*)));

        QListWidgetItem * item = new QListWidgetItem();
        ui->listWidget->addItem(item);
        ui->listWidget->setItemWidget(item, cell);
    }

    if (_currentSoundfontInfos.empty())
    {
        // Display "no results"
        ui->labelNoResult->show();
        ui->listWidget->hide();
    }
    else
    {
        // Display the list
        ui->listWidget->show();
        ui->labelNoResult->hide();
    }

    updateCellHeight();
}

void SoundfontBrowser::updateCellHeight()
{
    // Update size hints for all cells
    for (int i = 0; i < ui->listWidget->count(); i++)
    {
        QListWidgetItem * item = ui->listWidget->item(i);
        SoundfontCellFull* cell = dynamic_cast<SoundfontCellFull*>(ui->listWidget->itemWidget(item));
        item->setSizeHint(QSize(0, cell->heightForWidth(_lastWidth == -1 ? ui->listWidget->width() : _lastWidth)));
    }
}

void SoundfontBrowser::resizeEvent(QResizeEvent * event)
{
    _lastWidth = event->size().width();
    QWidget::resizeEvent(event);
    updateCellHeight();
}

void SoundfontBrowser::on_listWidget_itemSelectionChanged()
{
    for (int i = 0; i < ui->listWidget->count(); i++)
    {
        QListWidgetItem * item = ui->listWidget->item(i);
        (dynamic_cast<SoundfontCellFull*>(ui->listWidget->itemWidget(item)))->setActive(item->isSelected());
    }
}

void SoundfontBrowser::on_comboSort_currentIndexChanged(int index)
{
    Q_UNUSED(index)
    this->updateList2();
}

void SoundfontBrowser::on_pushClearSearch_clicked()
{
    ui->lineSearch->clear();
}

void SoundfontBrowser::keyPressEvent(QKeyEvent * event)
{
    // Key press event in the cell doesn't seem to work? So the code is here
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
    {
        for (int i = 0; i < ui->listWidget->count(); i++)
        {
            QListWidgetItem * item = ui->listWidget->item(i);
            if (item->isSelected())
            {
                SoundfontCellFull* cell = dynamic_cast<SoundfontCellFull*>(ui->listWidget->itemWidget(item));
                RepositoryManager::getInstance()->openSoundfont(cell->getSoundfontId(), false);
            }
        }
        event->accept();
        return;
    }

    QWidget::keyPressEvent(event);
}

void SoundfontBrowser::on_pushBecomePremium_clicked()
{
    if (UserManager::getInstance()->getConnectionState() == UserManager::CONNECTED)
        QDesktopServices::openUrl(QUrl("https://www.polyphone-soundfonts.com/en/subscribe"));
    else
        QDesktopServices::openUrl(QUrl("https://www.polyphone-soundfonts.com/en/create-an-account"));
}

void SoundfontBrowser::userStatusChanged(UserManager::ConnectionState state)
{
    ui->pushBecomePremium->setVisible(state != UserManager::CONNECTED_PREMIUM);
}
