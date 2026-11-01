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

#include "directorybrowser.h"
#include "ui_directorybrowser.h"
#include "contextmanager.h"
#include "directoryfiledata.h"
#include "customsplitter.h"
#include "soundfontmanager.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QKeyEvent>

DirectoryBrowser::DirectoryBrowser(QWidget *parent) : Tab(parent),
    ui(new Ui::DirectoryBrowser),
    _watcher(nullptr)
{
    ui->setupUi(this);

    // Style: top part
    QString highlightedBackground = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND).name();
    QString highlightedText = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT).name();
    QString border = ContextManager::theme()->getColor(ThemeManager::BORDER).name();

    ui->pushRetry->setIcon(ContextManager::theme()->getColoredSvg(":/icons/reload.svg", QSize(16, 16), ThemeManager::HIGHLIGHTED_TEXT));
    ui->widgetColored->setStyleSheet("QWidget{background-color:" + highlightedBackground + "}");
    ui->widgetColored2->setStyleSheet("QWidget{background-color:" + highlightedBackground + "}");
    QString titleStyleSheet = "QLabel#labelFilters, QFrame#frameTop{background-color:" + highlightedBackground + ";color:" + highlightedText+ "}";
    ui->frameTop->setStyleSheet(titleStyleSheet);
    ui->lineSearch->setStyleSheet("QLineEdit{background-color:" + highlightedText + ";color:" + highlightedBackground + ";border:0;border-radius:2px;}");

    // Style: center part
    ui->pushRetry->setStyleSheet("QPushButton{background-color:" + highlightedBackground + ";border-radius:5px;padding:5px}");
    QColor color = ThemeManager::mix(
        ContextManager::theme()->getColor(ThemeManager::LIST_BACKGROUND),
        ContextManager::theme()->getColor(ThemeManager::LIST_TEXT), 0.5);
    ui->labelNoResults->setStyleSheet("QLabel{color:" + color.name() + ";border:1px solid " + border + ";border-top:0;border-right:0;border-bottom:0}");
    ui->listView->hide();

    // Style: right part
    QMap<QString, QString> replacement;
    replacement["currentColor"] = ContextManager::theme()->getFixedColor(ThemeManager::YELLOW, ThemeManager::WINDOW_BACKGROUND).name();
    ui->iconSample->setPixmap(ContextManager::theme()->getColoredSvg(":/icons/sample.svg", QSize(16, 16), replacement));
    replacement["currentColor"] = ContextManager::theme()->getFixedColor(ThemeManager::BLUE, ThemeManager::WINDOW_BACKGROUND).name();
    ui->iconInstrument->setPixmap(ContextManager::theme()->getColoredSvg(":/icons/instrument.svg", QSize(16, 16), replacement));
    replacement["currentColor"] = ContextManager::theme()->getFixedColor(ThemeManager::RED, ThemeManager::WINDOW_BACKGROUND).name();
    ui->iconPreset->setPixmap(ContextManager::theme()->getColoredSvg(":/icons/preset.svg", QSize(16, 16), replacement));
    ui->pushShowSamples->setStyleSheet("QPushButton:focus {border: 2px solid " + highlightedBackground + "; border-radius: 2px;}");
    updateShowIcon(ui->pushShowSamples);
    ui->pushShowInstruments->setStyleSheet("QPushButton:focus {border: 2px solid " + highlightedBackground + "; border-radius: 2px;}");
    updateShowIcon(ui->pushShowInstruments);
    ui->pushShowPresets->setStyleSheet("QPushButton:focus {border: 2px solid " + highlightedBackground + "; border-radius: 2px;}");
    updateShowIcon(ui->pushShowPresets);

    // Connections
    connect(ui->widgetDisplayMenu, SIGNAL(displayOptionsChanged(int)), ui->listView, SLOT(setDisplayOptions(int)));
    connect(ui->lineSearch, SIGNAL(textEdited(QString)), ui->listView, SLOT(setFilter(QString)));
    connect(ui->lineSearch, SIGNAL(textEdited(QString)), ui->listSamples, SLOT(setFilter(QString)));
    connect(ui->lineSearch, SIGNAL(textEdited(QString)), ui->listInstruments, SLOT(setFilter(QString)));
    connect(ui->lineSearch, SIGNAL(textEdited(QString)), ui->listPresets, SLOT(setFilter(QString)));
    connect(ui->listView, SIGNAL(contentChanged()), this, SLOT(onContentChanged()));
    connect(ui->listView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(onSelectionChanged(QItemSelection,QItemSelection)));
    connect(ui->listView, SIGNAL(renameRequested(QString)), this, SLOT(onRenameRequested(QString)));
    connect(ui->listView, SIGNAL(deleteRequested(QString)), this, SLOT(onDeleteRequested(QString)));
    connect(ui->listSamples->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(onSmplSelectionChanged(QItemSelection,QItemSelection)));
    connect(ui->listInstruments->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(onInstSelectionChanged(QItemSelection,QItemSelection)));
    connect(ui->listPresets->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(onPrstSelectionChanged(QItemSelection,QItemSelection)));
    connect(ui->listSamples, SIGNAL(itemDoubleClicked(QString,EltID)), this, SIGNAL(itemDoubleClicked(QString,EltID)));
    connect(ui->listInstruments, SIGNAL(itemDoubleClicked(QString,EltID)), this, SIGNAL(itemDoubleClicked(QString,EltID)));
    connect(ui->listPresets, SIGNAL(itemDoubleClicked(QString,EltID)), this, SIGNAL(itemDoubleClicked(QString,EltID)));
    ui->lineSearch->installEventFilter(this);
    ui->listView->installEventFilter(this);
    ui->pushShowSamples->installEventFilter(this);
    ui->listSamples->installEventFilter(this);
    ui->pushShowInstruments->installEventFilter(this);
    ui->listInstruments->installEventFilter(this);
    ui->pushShowPresets->installEventFilter(this);
    ui->listPresets->installEventFilter(this);

    // Splitter
    CustomSplitter * splitter = new CustomSplitter(this, ui->widgetLeft, ui->widgetRight, "directory_browser_splitter_sizes");
    QGridLayout * layout = dynamic_cast<QGridLayout *>(ui->pageSoundfonts->layout());
    layout->addWidget(splitter, 1, 0);
    splitter->setHandleWidth(6);

    // Column header and initial display options
    ui->widgetHeader->setListView(ui->listView);
    ui->listView->setDisplayOptions(ui->widgetDisplayMenu->displayOptions());

    // Sections expanded or collapsed on the right
    ui->pushShowSamples->setChecked(ContextManager::configuration()->getValue(ConfManager::SECTION_DISPLAY, "directory_browser_show_samples", true).toBool());
    ui->pushShowInstruments->setChecked(ContextManager::configuration()->getValue(ConfManager::SECTION_DISPLAY, "directory_browser_show_instruments", true).toBool());
    ui->pushShowPresets->setChecked(ContextManager::configuration()->getValue(ConfManager::SECTION_DISPLAY, "directory_browser_show_presets", true).toBool());
}

DirectoryBrowser::~DirectoryBrowser()
{
    delete ui;
}

void DirectoryBrowser::initialize(QString dirPath)
{
    _dirPath = dirPath;
    //ui->spinner->startAnimation();

    // Directory checks
    QDir dir(dirPath);
    if (!dir.exists())
    {
        ui->labelError->setText(tr("The directory \"%1\" does not exist.").arg(dirPath));
        ui->stackedWidget->setCurrentIndex(1);
        return;
    }

    if (!dir.isReadable())
    {
        ui->labelError->setText(tr("The directory \"%1\" is not readable.").arg(dirPath));
        ui->stackedWidget->setCurrentIndex(1);
        return;
    }

    // Files already open
    QMap<QString, int> openedFilesWithId;
    EltID idSf2(elementSf2);
    SoundfontManager * sm = SoundfontManager::getInstance();
    foreach (int index, sm->getSiblings(idSf2))
    {
        idSf2.indexSf2 = index;
        openedFilesWithId[sm->getQstr(idSf2, champ_filenameInitial)] = index;
    }

    // Browse the files
    QMap<QString, QDateTime> newFiles;
    QFileInfoList files = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
    for (const QFileInfo &fileInfo : files)
    {
        newFiles[fileInfo.absoluteFilePath()] = fileInfo.lastModified();
        if (_currentFiles.contains(fileInfo.absoluteFilePath()))
        {
            if (_currentFiles[fileInfo.absoluteFilePath()] != newFiles[fileInfo.absoluteFilePath()])
            {
                int currentSf2Id = openedFilesWithId.contains(fileInfo.absoluteFilePath()) ?
                                       openedFilesWithId[fileInfo.absoluteFilePath()] : -1;
                ui->listView->updateFile(new DirectoryFileData(fileInfo, currentSf2Id));
            }
            _currentFiles.remove(fileInfo.absoluteFilePath());
        }
        else
        {
            int currentSf2Id = openedFilesWithId.contains(fileInfo.absoluteFilePath()) ?
                                   openedFilesWithId[fileInfo.absoluteFilePath()] : -1;
            ui->listView->addFile(new DirectoryFileData(fileInfo, currentSf2Id));
        }
    }
    foreach (QString absoluteFilePath, _currentFiles.keys())
        ui->listView->removeFile(absoluteFilePath);
    _currentFiles = newFiles;
    ui->stackedWidget->setCurrentIndex(2);

    if (_watcher == nullptr)
    {
        _watcher = new QFileSystemWatcher();
        _watcher->addPath(dirPath);
        connect(_watcher, SIGNAL(directoryChanged(QString)), this, SLOT(onDirectoryChanged(QString)));
    }
}

void DirectoryBrowser::on_pushRetry_clicked()
{
    this->initialize(_dirPath);
}

void DirectoryBrowser::onDirectoryChanged(const QString &path)
{
    Q_UNUSED(path)
    this->initialize(_dirPath);
}

void DirectoryBrowser::onContentChanged()
{
    if (ui->listView->model()->rowCount() > 0)
    {
        ui->labelNoResults->hide();
        ui->listView->show();
    }
    else
    {
        ui->listView->hide();
        ui->labelNoResults->show();
    }

    // The header width depends on the list visibility
    ui->widgetHeader->update();
}

void DirectoryBrowser::onSelectionChanged(QItemSelection selected, QItemSelection deselected)
{
    Q_UNUSED(selected)
    Q_UNUSED(deselected)

    QModelIndex index = ui->listView->currentIndex();
    const DirectoryFileData * d = index.data(Qt::UserRole).value<const DirectoryFileData *>();
    if (d == nullptr)
    {
        ui->listSamples->clear();
        ui->listInstruments->clear();
        ui->listPresets->clear();
    }
    else
    {
        ui->listSamples->setData(d->getSampleDetails(), d->getPath(), elementSmpl);
        ui->listInstruments->setData(d->getInstrumentDetails(), d->getPath(), elementInst);
        ui->listPresets->setData(d->getPresetDetails(), d->getPath(), elementPrst);
    }
}

void DirectoryBrowser::on_listView_activated(const QModelIndex &index)
{
    const DirectoryFileData * d = index.data(Qt::UserRole).value<const DirectoryFileData *>();
    if (d == nullptr ||
        d->getStatus() == DirectoryFileData::NOT_INITIALIZED ||
        d->getStatus() == DirectoryFileData::NOT_READABLE ||
        d->getStatus() == DirectoryFileData::NOT_OPENABLE)
        return;

    // No initial selection: the startup view will be used
    emit itemDoubleClicked(d->getPath(), EltID());
}

void DirectoryBrowser::onRenameRequested(QString path)
{
    QFileInfo info(path);
    QString currentName = info.fileName();

    // Dialog
    QInputDialog inputDialog(this);
    inputDialog.setWindowTitle(tr("Rename"));
    inputDialog.setLabelText(tr("New name:"));
    inputDialog.setTextValue(currentName);
    inputDialog.setCancelButtonText(tr("&Cancel"));
    inputDialog.setOkButtonText(tr("&Ok"));
    if (inputDialog.exec() != QDialog::Accepted)
        return;
    QString newName = inputDialog.textValue();
    if (newName.isEmpty() || newName == currentName)
        return;

    QDir dir = info.dir();
    QString newPath = dir.filePath(newName);
    if (QFile::exists(newPath))
    {
        QMessageBox::warning(this, tr("Warning"), tr("A file with this name already exists."));
        return;
    }

    if (!QFile::rename(path, newPath))
        QMessageBox::warning(this, tr("Warning"), tr("Cannot rename file \"%1\".").arg(currentName));
}

void DirectoryBrowser::onDeleteRequested(QString path)
{
    QFileInfo info(path);

    QMessageBox msgBox(this);
    msgBox.setWindowTitle(tr("Confirm deletion"));
    msgBox.setText(tr("Are you sure you want to delete file \"%1\"?").arg(info.fileName()));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.button(QMessageBox::Yes)->setText(tr("&Yes"));
    msgBox.button(QMessageBox::No)->setText(tr("&No"));
    msgBox.setIcon(QMessageBox::Question);
    if (msgBox.exec() != QMessageBox::Yes)
        return;

    if (!QFile::moveToTrash(path))
        QMessageBox::warning(this, tr("Warning"), tr("Cannot delete file \"%1\".").arg(info.fileName()));
}

void DirectoryBrowser::onSmplSelectionChanged(QItemSelection selected, QItemSelection deselected)
{
    Q_UNUSED(selected)
    Q_UNUSED(deselected)

    ui->listInstruments->selectionModel()->blockSignals(true);
    ui->listInstruments->clearSelection();
    ui->listInstruments->selectionModel()->blockSignals(false);
    ui->listInstruments->viewport()->update();

    ui->listPresets->selectionModel()->blockSignals(true);
    ui->listPresets->clearSelection();
    ui->listPresets->selectionModel()->blockSignals(false);
    ui->listPresets->viewport()->update();
}

void DirectoryBrowser::onInstSelectionChanged(QItemSelection selected, QItemSelection deselected)
{
    Q_UNUSED(selected)
    Q_UNUSED(deselected)

    ui->listSamples->selectionModel()->blockSignals(true);
    ui->listSamples->clearSelection();
    ui->listSamples->selectionModel()->blockSignals(false);
    ui->listSamples->viewport()->update();

    ui->listPresets->selectionModel()->blockSignals(true);
    ui->listPresets->clearSelection();
    ui->listPresets->selectionModel()->blockSignals(false);
    ui->listPresets->viewport()->update();
}

void DirectoryBrowser::onPrstSelectionChanged(QItemSelection selected, QItemSelection deselected)
{
    Q_UNUSED(selected)
    Q_UNUSED(deselected)

    ui->listSamples->selectionModel()->blockSignals(true);
    ui->listSamples->clearSelection();
    ui->listSamples->selectionModel()->blockSignals(false);
    ui->listSamples->viewport()->update();

    ui->listInstruments->selectionModel()->blockSignals(true);
    ui->listInstruments->clearSelection();
    ui->listInstruments->selectionModel()->blockSignals(false);
    ui->listInstruments->viewport()->update();
}

void DirectoryBrowser::onActionRequired(TabAction action)
{
    switch (action)
    {
    case Tab::SEARCH:
        ui->lineSearch->selectAll();
        ui->lineSearch->setFocus();
        break;
    default:
        break;
    }
}

void DirectoryBrowser::on_pushShowSamples_toggled(bool checked)
{
    updateShowIcon(ui->pushShowSamples);
    ui->listSamples->setVisible(checked);
    updateVerticalSpacer();
    ContextManager::configuration()->setValue(ConfManager::SECTION_DISPLAY, "directory_browser_show_samples", checked);
}

void DirectoryBrowser::on_pushShowInstruments_toggled(bool checked)
{
    updateShowIcon(ui->pushShowInstruments);
    ui->listInstruments->setVisible(checked);
    updateVerticalSpacer();
    ContextManager::configuration()->setValue(ConfManager::SECTION_DISPLAY, "directory_browser_show_instruments", checked);
}

void DirectoryBrowser::on_pushShowPresets_toggled(bool checked)
{
    updateShowIcon(ui->pushShowPresets);
    ui->listPresets->setVisible(checked);
    updateVerticalSpacer();
    ContextManager::configuration()->setValue(ConfManager::SECTION_DISPLAY, "directory_browser_show_presets", checked);
}

void DirectoryBrowser::updateVerticalSpacer()
{
    if (ui->listSamples->isHidden() && ui->listInstruments->isHidden() && ui->listPresets->isHidden())
        ui->verticalSpacer->changeSize(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    else if (ui->listPresets->isHidden())
        ui->verticalSpacer->changeSize(20, 6, QSizePolicy::Minimum, QSizePolicy::Fixed);
    else
        ui->verticalSpacer->changeSize(0, 0, QSizePolicy::Minimum, QSizePolicy::Fixed);
    ui->verticalLayout_2->invalidate();
}

void DirectoryBrowser::updateShowIcon(QPushButton * button)
{
    button->setIcon(ContextManager::theme()->getColoredSvg(button->isChecked() ? ":/icons/arrow_down.svg" : ":/icons/arrow_up.svg",
                                                           QSize(16, 16), ThemeManager::WINDOW_TEXT));
}

bool DirectoryBrowser::eventFilter(QObject *obj, QEvent *event)
{
    // Navigation with arrows between widgets
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        switch (keyEvent->key())
        {
        case Qt::Key_Up:
            if (obj == ui->listView)
            {
                QModelIndex index = ui->listView->currentIndex();
                if (index.isValid() && index.row() == 0)
                {
                    ui->lineSearch->selectAll();
                    ui->lineSearch->setFocus();
                    return true;
                }
            }
            else if (obj == ui->pushShowSamples)
            {
                ui->listView->setFocus();
                return true;
            }
            else if (obj == ui->pushShowInstruments)
            {
                ui->pushShowSamples->setFocus();
                return true;
            }
            else if (obj == ui->pushShowPresets)
            {
                ui->pushShowInstruments->setFocus();
                return true;
            }
            break;
        case Qt::Key_Down:
            if (obj == ui->lineSearch)
            {
                // Select the first element in the list (if any)
                if (ui->listView->model() && ui->listView->model()->rowCount() > 0)
                {
                    ui->listView->setFocus();
                    if (!ui->listView->selectionModel()->hasSelection())
                        ui->listView->setCurrentIndex(ui->listView->model()->index(0, 0));
                }
                return true;
            }
            else if (obj == ui->pushShowSamples)
            {
                ui->pushShowInstruments->setFocus();
                return true;
            }
            else if (obj == ui->pushShowInstruments)
            {
                ui->pushShowPresets->setFocus();
                return true;
            }
            else if (obj == ui->pushShowPresets)
            {
                return true;
            }
            break;
        case Qt::Key_Right:
            if (obj == ui->listView)
            {
                ui->pushShowSamples->setFocus();
                return true;
            }
            else if (obj == ui->pushShowSamples)
            {
                if (ui->pushShowSamples->isChecked())
                {
                    if (ui->listSamples->model()->rowCount() > 0)
                    {
                        ui->listSamples->setFocus();
                        if (!ui->listSamples->selectionModel()->hasSelection())
                            ui->listSamples->setCurrentIndex(ui->listSamples->model()->index(0, 0));
                    }
                }
                else
                    ui->pushShowSamples->setChecked(true);
                return true;
            }
            else if (obj == ui->pushShowInstruments)
            {
                if (ui->pushShowInstruments->isChecked())
                {
                    if (ui->listInstruments->model()->rowCount() > 0)
                    {
                        ui->listInstruments->setFocus();
                        if (!ui->listInstruments->selectionModel()->hasSelection())
                            ui->listInstruments->setCurrentIndex(ui->listInstruments->model()->index(0, 0));
                    }
                }
                else
                    ui->pushShowInstruments->setChecked(true);
                return true;
            }
            else if (obj == ui->pushShowPresets)
            {
                if (ui->pushShowPresets->isChecked())
                {
                    if (ui->listPresets->model()->rowCount() > 0)
                    {
                        ui->listPresets->setFocus();
                        if (!ui->listPresets->selectionModel()->hasSelection())
                            ui->listPresets->setCurrentIndex(ui->listPresets->model()->index(0, 0));
                    }
                }
                else
                    ui->pushShowPresets->setChecked(true);
                return true;
            }
            break;
        case Qt::Key_Left:
            if (obj == ui->listView)
            {
                ui->lineSearch->selectAll();
                ui->lineSearch->setFocus();
                return true;
            }
            else if (obj == ui->pushShowSamples)
            {
                ui->pushShowSamples->setChecked(false);
                return true;
            }
            else if (obj == ui->pushShowInstruments)
            {
                ui->pushShowInstruments->setChecked(false);
                return true;
            }
            else if (obj == ui->pushShowPresets)
            {
                ui->pushShowPresets->setChecked(false);
                return true;
            }
            else if (obj == ui->listSamples)
            {
                ui->pushShowSamples->setFocus();
                return true;
            }
            else if (obj == ui->listInstruments)
            {
                ui->pushShowInstruments->setFocus();
                return true;
            }
            else if (obj == ui->listPresets)
            {
                ui->pushShowPresets->setFocus();
                return true;
            }
            break;
        }
    }

    return Tab::eventFilter(obj, event);
}