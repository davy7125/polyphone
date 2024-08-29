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

#include "soundfontbrowser.h"
#include "ui_soundfontbrowser.h"
#include "contextmanager.h"
#include "soundfontfilter.h"
#include "repositorymanager.h"
#include "soundfontcellfull.h"
#include "utils.h"
#include <QResizeEvent>
#include <QDesktopServices>
#include <QScrollBar>

const int SoundfontBrowser::ITEMS_PER_PAGE = 25;

SoundfontBrowser::SoundfontBrowser(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SoundfontBrowser),
    _loadingFilter(false),
    _currentPage(0)
{
    ui->setupUi(this);

    // Style
    QString highlightedBackground = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND).name();
    QString highlightedText = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT).name();
    QString border = ContextManager::theme()->getColor(ThemeManager::BORDER).name();

    ui->pushRetry->setIcon(ContextManager::theme()->getColoredSvg(":/icons/reload.svg", QSize(16, 16), ThemeManager::HIGHLIGHTED_TEXT));
    ui->pushRetry->setStyleSheet("QPushButton{background-color:" + highlightedBackground + ";border-radius:5px;padding:5px}");
    ui->widgetColored->setStyleSheet("QWidget{background-color:" + highlightedBackground + "}");
    ui->widgetColored2->setStyleSheet("QWidget{background-color:" + highlightedBackground + "}");
    QString titleStyleSheet = "QLabel#labelFilters, QFrame#frameTitle, QFrame#frameSearch{background-color:" + highlightedBackground + ";color:" + highlightedText+ "}";
    ui->frameTitle->setStyleSheet(titleStyleSheet);
    ui->frameSearch->setStyleSheet(titleStyleSheet);
    ui->pushBecomePremium->setStyleSheet("QPushButton{border:1px solid " + border + ";border-top:0;border-right:0;padding:4px;color:" +
                                         ContextManager::theme()->getFixedColor(ThemeManager::RED, ThemeManager::BUTTON_BACKGROUND).name() + "}");
    ui->pushResetFilters->setStyleSheet("QPushButton{background-color:" + highlightedText + ";color:" + highlightedBackground +
                                        ";border-radius:2px;border:0;padding:2px 5px} QPushButton:hover{background-color:" +
                                        ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT, ThemeManager::HOVERED).name() + "}");
    ui->lineSearch->setStyleSheet("QLineEdit{background-color:" + highlightedText + ";color:" + highlightedBackground + ";border:0;border-radius:2px;}");
    QColor color = ThemeManager::mix(
                ContextManager::theme()->getColor(ThemeManager::LIST_BACKGROUND),
                ContextManager::theme()->getColor(ThemeManager::LIST_TEXT), 0.5);
    ui->labelNoResult->setStyleSheet("QLabel{color:" + color.name() + ";border:1px solid " + border + ";border-top:0;border-right:0;border-bottom:0}");
    ui->listWidget->setStyleSheet("QListWidget{border:1px solid " + border + ";border-top:0;border-right:0;border-bottom:0}" +
                                  "QListWidget::item:selected {background-color: " + highlightedBackground + "}" +
                                  "QAbstractSckrollArea{margin: 100px; padding: 100px;}");

    // Pagination style
    ui->framePagination->setStyleSheet("QFrame#framePagination{background-color: " +
                                       ContextManager::theme()->getColor(ThemeManager::WINDOW_BACKGROUND).name() +
                                       ";color: " + ContextManager::theme()->getColor(ThemeManager::WINDOW_TEXT).name() +
                                       ";border: 1px solid " + border + ";border-radius: 3px;}");
    ui->framePagination->setParent(this);
    ui->pushGoPrevious->setIcon(ContextManager::theme()->getColoredSvg(":/icons/arrow_left.svg", QSize(30, 30), ThemeManager::WINDOW_TEXT));
    ui->pushGoNext->setIcon(ContextManager::theme()->getColoredSvg(":/icons/arrow_right.svg", QSize(30, 30), ThemeManager::WINDOW_TEXT));

    // Connection with the repository manager
    RepositoryManager * rm = RepositoryManager::getInstance();
    connect(rm, SIGNAL(initializing()), this, SLOT(initialize()));
    connect(rm, SIGNAL(ready(QString)), this, SLOT(soundfontListAvailable(QString)));

    // Filter connection
    connect(ui->lineSearch, SIGNAL(textChanged(QString)), this, SLOT(updateFilter()));
    connect(ui->filterCategory, SIGNAL(selectionChanged()), this, SLOT(updateFilter()));
    connect(ui->filterLicense, SIGNAL(selectionChanged()), this, SLOT(updateFilter()));
    connect(ui->filterSampleSource, SIGNAL(selectionChanged()), this, SLOT(updateFilter()));
    connect(ui->filterTimbre, SIGNAL(selectionChanged()), this, SLOT(updateFilter()));
    connect(ui->filterArticulation, SIGNAL(selectionChanged()), this, SLOT(updateFilter()));
    connect(ui->filterGenre, SIGNAL(selectionChanged()), this, SLOT(updateFilter()));
    connect(ui->filterMidiStandard, SIGNAL(selectionChanged()), this, SLOT(updateFilter()));
    connect(ui->filterTag, SIGNAL(selectionChanged()), this, SLOT(updateFilter()));

    // Connection with the user manager
    connect(UserManager::getInstance(), SIGNAL(connectionStateChanged(UserManager::ConnectionState)),
            this, SLOT(userStatusChanged(UserManager::ConnectionState)));

    // Other connection
    connect(ui->widgetSortMenu, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboSortCurrentIndexChanged(int)));
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
    ui->filterLicense->addElement(tr("commercial use"), static_cast<int>(SoundfontInformation::LicenseFlag::COMMERCIAL_USE));
    ui->filterLicense->addElement(tr("share after editing"), static_cast<int>(SoundfontInformation::LicenseFlag::SHARE_MODIFIED));

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
    updateFilter();
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

void SoundfontBrowser::updateFilter()
{
    if (_loadingFilter)
        return;

    // Build a filter
    SoundfontFilter * filter = this->getFilter();

    // Filter soundfonts
    _currentSoundfontInfos = RepositoryManager::getInstance()->getSoundfontInformation(filter);
    delete filter;

    // Back to the first page
    _currentPage = 0;

    // Next step: sort the items
    updateSort();
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

void SoundfontBrowser::updateSort()
{
    // Sort
    switch (ui->widgetSortMenu->currentIndex())
    {
    case 0:
        std::sort(_currentSoundfontInfos.begin(), _currentSoundfontInfos.end(), sortByDate);
        break;
    case 1:
        std::sort(_currentSoundfontInfos.begin(), _currentSoundfontInfos.end(), sortByDownloads);
        break;
    case 2:
        std::sort(_currentSoundfontInfos.begin(), _currentSoundfontInfos.end(), sortByScore);
        break;
    case 3:
        std::sort(_currentSoundfontInfos.begin(), _currentSoundfontInfos.end(), sortByTitle);
        break;
    }

    // Next step: fill the list according to the page
    updatePage();
}

void SoundfontBrowser::updatePage()
{
    // Current page / Number of pages, previous and next buttons
    ui->labelPagination->setText(QString("%1 / %2").arg(_currentPage + 1).arg(1 + (_currentSoundfontInfos.count() - 1) / ITEMS_PER_PAGE));
    ui->pushGoPrevious->setEnabled(_currentPage > 0);
    ui->pushGoNext->setEnabled(_currentPage < (_currentSoundfontInfos.count() - 1) / ITEMS_PER_PAGE);

    // Fill with the soundfonts
    ui->listWidget->clear();
    for (int i = _currentPage * ITEMS_PER_PAGE; i < qMin((_currentPage + 1) * ITEMS_PER_PAGE, _currentSoundfontInfos.count()); i++)
    {
        SoundfontCellFull * cell = new SoundfontCellFull(_currentSoundfontInfos[i], this);
        connect(cell, SIGNAL(itemClicked(SoundfontFilter*)), this, SLOT(applyFilter(SoundfontFilter*)));

        QListWidgetItem * item = new QListWidgetItem();
        ui->listWidget->addItem(item);
        ui->listWidget->setItemWidget(item, cell);
    }

    if (_currentSoundfontInfos.empty())
    {
        // Display "no results"
        ui->labelNoResult->show();
        ui->framePagination->hide();
        ui->listWidget->hide();
    }
    else
    {
        // Display the list
        ui->listWidget->show();
        ui->framePagination->show();
        ui->labelNoResult->hide();
    }

    // Next step: adapt the height of the cells
    updateCellHeight();
}

void SoundfontBrowser::updateCellHeight()
{
    // Update size hints for all cells
    int viewPortWidth = ui->listWidget->viewport()->width();
    for (int i = 0; i < ui->listWidget->count(); i++)
    {
        QListWidgetItem * item = ui->listWidget->item(i);
        SoundfontCellFull* cell = dynamic_cast<SoundfontCellFull*>(ui->listWidget->itemWidget(item));
        cell->setMaximumWidth(viewPortWidth);
        item->setSizeHint(QSize(0, cell->heightForWidth(viewPortWidth)));
    }

    // Pagination location
    ui->framePagination->move(this->width() - ui->framePagination->width() - ui->listWidget->verticalScrollBar()->width() - 5,
                              this->height() - ui->framePagination->height() - 5);
}

void SoundfontBrowser::showEvent(QShowEvent * event)
{
    QWidget::showEvent(event);
    updateCellHeight();
}

void SoundfontBrowser::resizeEvent(QResizeEvent * event)
{
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

void SoundfontBrowser::onComboSortCurrentIndexChanged(int index)
{
    Q_UNUSED(index)
    updateSort();
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
        QDesktopServices::openUrl(QUrl("https://www.polyphone.io/premium"));
    else
        QDesktopServices::openUrl(QUrl("https://www.polyphone.io/register"));
}

void SoundfontBrowser::userStatusChanged(UserManager::ConnectionState state)
{
    ui->pushBecomePremium->setVisible(state != UserManager::CONNECTED_PREMIUM && state != UserManager::CONNECTED_ADMIN);
}

void SoundfontBrowser::on_pushGoPrevious_clicked()
{
    if (_currentPage > 0)
    {
        _currentPage--;
        updatePage();
    }
}

void SoundfontBrowser::on_pushGoNext_clicked()
{
    if (_currentPage < (_currentSoundfontInfos.count() - 1) / ITEMS_PER_PAGE)
    {
        _currentPage++;
        updatePage();
    }
}
