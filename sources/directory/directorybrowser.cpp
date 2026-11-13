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
#include "directorypreview.h"
#include "editortoolbar.h"
#include "dialogkeyboard.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QKeyEvent>

DirectoryBrowser::DirectoryBrowser(DialogKeyboard * dialogKeyboard, QWidget *parent) : Tab(parent),
    ui(new Ui::DirectoryBrowser),
    _watcher(nullptr),
    _dialogKeyboard(dialogKeyboard),
    _preview(new DirectoryPreview(dialogKeyboard, this)),
    _keyboardDisplayUpdateScheduled(false),
    _elementTypeToSelect(elementUnknown)
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
    ui->separator->setStyleSheet("QFrame{border: 1px solid " + highlightedText + ";margin: 7px 3px;}");

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

    // MIDI player, virtual keyboard and recorder, in the same state as the buttons of the editor (the elements cannot be
    // played in player mode, but the MIDI player can play the file selected)
    ui->pushShowMidiPlayer->initialize(tr("MIDI player"), ":/icons/MIDI.svg");
    ui->pushShowKeyboard->initialize(tr("Virtual keyboard"), ":/icons/piano.svg");
    ui->pushShowRecorder->initialize(tr("Recorder"), ":/icons/recorder.svg");
    EditorToolBar::addMidiPlayerButton(ui->pushShowMidiPlayer);
    if (_dialogKeyboard != nullptr)
    {
        EditorToolBar::addKeyboardButton(ui->pushShowKeyboard);
        EditorToolBar::addRecorderButton(ui->pushShowRecorder);
    }
    else
    {
        ui->pushShowKeyboard->hide();
        ui->pushShowRecorder->hide();
    }

    // Connections
    connect(ui->pushDisplay, SIGNAL(displayOptionsChanged(int)), this, SLOT(onDisplayOptionsChanged(int)));
    connect(ui->lineSearch, SIGNAL(textEdited(QString)), ui->listView, SLOT(setFilter(QString)));
    connect(ui->lineSearch, SIGNAL(textEdited(QString)), ui->listSamples, SLOT(setFilter(QString)));
    connect(ui->lineSearch, SIGNAL(textEdited(QString)), ui->listInstruments, SLOT(setFilter(QString)));
    connect(ui->lineSearch, SIGNAL(textEdited(QString)), ui->listPresets, SLOT(setFilter(QString)));
    connect(ui->listView, SIGNAL(contentChanged()), this, SLOT(onContentChanged()));
    connect(ui->listView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(onSelectionChanged(QItemSelection,QItemSelection)));
    connect(ui->listView->model(), SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(onFileDataChanged(QModelIndex,QModelIndex)));
    connect(ui->listView, SIGNAL(renameRequested(QString)), this, SLOT(onRenameRequested(QString)));
    connect(ui->listView, SIGNAL(deleteRequested(QString)), this, SLOT(onDeleteRequested(QString)));
    connect(ui->listSamples->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(onSmplSelectionChanged(QItemSelection,QItemSelection)));
    connect(ui->listInstruments->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(onInstSelectionChanged(QItemSelection,QItemSelection)));
    connect(ui->listPresets->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(onPrstSelectionChanged(QItemSelection,QItemSelection)));
    connect(ui->listSamples, SIGNAL(itemDoubleClicked(QString,EltID)), this, SIGNAL(itemDoubleClicked(QString,EltID)));
    connect(ui->listInstruments, SIGNAL(itemDoubleClicked(QString,EltID)), this, SIGNAL(itemDoubleClicked(QString,EltID)));
    connect(ui->listPresets, SIGNAL(itemDoubleClicked(QString,EltID)), this, SIGNAL(itemDoubleClicked(QString,EltID)));

    // Display of the virtual keyboard: selection, elements displayed (filter, sort, scan)
    connect(ui->lineSearch, &QLineEdit::textEdited, this, &DirectoryBrowser::scheduleKeyboardDisplayUpdate);
    QList<QAbstractItemView *> views;
    views << ui->listView << ui->listSamples << ui->listInstruments << ui->listPresets;
    foreach (QAbstractItemView * view, views)
    {
        connect(view->model(), &QAbstractItemModel::rowsInserted, this, &DirectoryBrowser::scheduleKeyboardDisplayUpdate);
        connect(view->model(), &QAbstractItemModel::rowsRemoved, this, &DirectoryBrowser::scheduleKeyboardDisplayUpdate);
        connect(view->model(), &QAbstractItemModel::layoutChanged, this, &DirectoryBrowser::scheduleKeyboardDisplayUpdate);
        connect(view->model(), &QAbstractItemModel::modelReset, this, &DirectoryBrowser::scheduleKeyboardDisplayUpdate);
    }

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
    onDisplayOptionsChanged(ui->pushDisplay->displayOptions());

    // Sections expanded or collapsed on the right
    ui->pushShowSamples->setChecked(ContextManager::configuration()->getValue(ConfManager::SECTION_DISPLAY, "directory_browser_show_samples", true).toBool());
    ui->pushShowInstruments->setChecked(ContextManager::configuration()->getValue(ConfManager::SECTION_DISPLAY, "directory_browser_show_instruments", true).toBool());
    ui->pushShowPresets->setChecked(ContextManager::configuration()->getValue(ConfManager::SECTION_DISPLAY, "directory_browser_show_presets", true).toBool());
}

DirectoryBrowser::~DirectoryBrowser()
{
    EditorToolBar::removeMidiPlayerButton(ui->pushShowMidiPlayer);
    if (_dialogKeyboard != nullptr)
    {
        EditorToolBar::removeKeyboardButton(ui->pushShowKeyboard);
        EditorToolBar::removeRecorderButton(ui->pushShowRecorder);
    }
    delete _preview;
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

    // The soundfont played is released if its file changed
    if (_currentFiles.contains(_preview->getFile()))
    {
        QFileInfo fileInfo(_preview->getFile());
        if (!fileInfo.exists() || fileInfo.lastModified() != _currentFiles[_preview->getFile()])
            _preview->release();
    }

    // Files already open (not the soundfonts loaded by the explorer)
    QMap<QString, int> openedFilesWithId;
    EltID idSf2(elementSf2);
    SoundfontManager * sm = SoundfontManager::getInstance();
    foreach (int index, sm->getSiblings(idSf2))
    {
        if (sm->isTemporary(index))
            continue;
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

    // The lists have no selection anymore, the soundfont played is released if another file is selected. The file can be
    // loaded once it has been scanned.
    if (d == nullptr)
        _preview->setFile("", false, false);
    else
        _preview->setFile(d->getPath(), d->getStatus() == DirectoryFileData::NOT_INITIALIZED,
                          d->getStatus() == DirectoryFileData::OK);
    setPreviewElement(EltID(elementUnknown));
    selectFirstElementIfRequired(d);
    scheduleKeyboardDisplayUpdate();
}

void DirectoryBrowser::onFileDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    // The details of the current file are updated (scan over, file changed)
    QModelIndex index = ui->listView->currentIndex();
    if (index.isValid() && index.row() >= topLeft.row() && index.row() <= bottomRight.row())
        onSelectionChanged(QItemSelection(), QItemSelection());
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

    setPreviewElement(ui->listSamples->getSelectedElement());
    scheduleKeyboardDisplayUpdate();
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

    setPreviewElement(ui->listInstruments->getSelectedElement());
    scheduleKeyboardDisplayUpdate();
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

    setPreviewElement(ui->listPresets->getSelectedElement());
    scheduleKeyboardDisplayUpdate();
}

void DirectoryBrowser::onActionRequired(TabAction action)
{
    switch (action)
    {
    case Tab::SEARCH:
        ui->lineSearch->selectAll();
        ui->lineSearch->setFocus();
        break;
    case Tab::PREVIOUS_ELEMENT: case Tab::NEXT_ELEMENT: {
        // The selection changes, the focus staying where it is (virtual keyboard)
        int direction = (action == Tab::NEXT_ELEMENT ? 1 : -1);
        DirectoryElementListView * list = getSelectedList();
        if (list != nullptr)
        {
            int row = list->getSelectedRow() + direction;
            if (row >= 0 && row < list->getRowCount())
                list->selectRow(row);
        }
        else if (direction > 0 && (list = getFirstList()) != nullptr)
            list->selectRow(0); // Nothing selected: first element
    } break;
    case Tab::PREVIOUS_FILE: case Tab::NEXT_FILE: {
        QModelIndex index = ui->listView->currentIndex();
        int row = index.isValid() ? index.row() + (action == Tab::NEXT_FILE ? 1 : -1) :
                      (action == Tab::NEXT_FILE ? 0 : -1); // Nothing selected: first file
        if (row >= 0 && row < ui->listView->model()->rowCount())
        {
            // The first element of the type selected will be selected in the new file
            index = ui->listView->model()->index(row, 0);
            DirectoryElementListView * list = getSelectedList();
            const DirectoryFileData * d = index.data(Qt::UserRole).value<const DirectoryFileData *>();
            _elementTypeToSelect = (list != nullptr && d != nullptr) ? list->getType() : elementUnknown;
            _elementTypeToSelectPath = (d != nullptr ? d->getPath() : "");

            ui->listView->setCurrentIndex(index);
            ui->listView->scrollTo(index);
        }
    } break;
    default:
        break;
    }
}

void DirectoryBrowser::onDisplayOptionsChanged(int options)
{
    ui->listView->setDisplayOptions(options);
    ui->listSamples->setDetailsDisplayed(options & DirectoryDisplayMenu::SAMPLE_DETAILS);
    ui->listInstruments->setDetailsDisplayed(options & DirectoryDisplayMenu::INSTRUMENT_DETAILS);
    ui->listPresets->setDetailsDisplayed(options & DirectoryDisplayMenu::PRESET_DETAILS);
}

void DirectoryBrowser::on_pushShowSamples_toggled(bool checked)
{
    updateShowIcon(ui->pushShowSamples);
    scheduleKeyboardDisplayUpdate();
    ui->listSamples->setVisible(checked);
    updateVerticalSpacer();
    ContextManager::configuration()->setValue(ConfManager::SECTION_DISPLAY, "directory_browser_show_samples", checked);
}

void DirectoryBrowser::on_pushShowInstruments_toggled(bool checked)
{
    updateShowIcon(ui->pushShowInstruments);
    scheduleKeyboardDisplayUpdate();
    ui->listInstruments->setVisible(checked);
    updateVerticalSpacer();
    ContextManager::configuration()->setValue(ConfManager::SECTION_DISPLAY, "directory_browser_show_instruments", checked);
}

void DirectoryBrowser::on_pushShowPresets_toggled(bool checked)
{
    updateShowIcon(ui->pushShowPresets);
    scheduleKeyboardDisplayUpdate();
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

void DirectoryBrowser::on_pushShowKeyboard_clicked()
{
    emit keyboardDisplayChanged(ui->pushShowKeyboard->isChecked());
}

void DirectoryBrowser::on_pushShowRecorder_clicked()
{
    emit recorderDisplayChanged(ui->pushShowRecorder->isChecked());
}

void DirectoryBrowser::on_pushShowMidiPlayer_clicked()
{
    emit midiPlayerDisplayChanged(ui->pushShowMidiPlayer->isChecked());
}

void DirectoryBrowser::setPreviewElement(EltID id)
{
    _preview->setElement(id);
}

void DirectoryBrowser::showEvent(QShowEvent * event)
{
    Tab::showEvent(event);
    _preview->onShown();
    scheduleKeyboardDisplayUpdate();
}

DirectoryElementListView * DirectoryBrowser::getSelectedList()
{
    QList<DirectoryElementListView *> lists;
    lists << ui->listPresets << ui->listInstruments << ui->listSamples;
    foreach (DirectoryElementListView * list, lists)
        if (list->getSelectedRow() >= 0)
            return list;
    return nullptr;
}

DirectoryElementListView * DirectoryBrowser::getFirstList()
{
    QList<QPair<DirectoryElementListView *, QPushButton *>> lists;
    lists << QPair<DirectoryElementListView *, QPushButton *>(ui->listPresets, ui->pushShowPresets)
          << QPair<DirectoryElementListView *, QPushButton *>(ui->listInstruments, ui->pushShowInstruments)
          << QPair<DirectoryElementListView *, QPushButton *>(ui->listSamples, ui->pushShowSamples);
    for (int i = 0; i < lists.count(); i++)
        if (lists[i].second->isChecked() && lists[i].first->getRowCount() > 0)
            return lists[i].first;
    return nullptr;
}

void DirectoryBrowser::selectFirstElementIfRequired(const DirectoryFileData * d)
{
    if (_elementTypeToSelect == elementUnknown)
        return;

    // Another file selected meanwhile
    if (d == nullptr || d->getPath() != _elementTypeToSelectPath)
    {
        _elementTypeToSelect = elementUnknown;
        return;
    }

    DirectoryElementListView * list = (_elementTypeToSelect == elementSmpl ? ui->listSamples :
                                           (_elementTypeToSelect == elementInst ? ui->listInstruments : ui->listPresets));
    if (list->getRowCount() > 0)
    {
        _elementTypeToSelect = elementUnknown;
        list->selectRow(0);
    }
    else if (d->getStatus() != DirectoryFileData::NOT_INITIALIZED)
        _elementTypeToSelect = elementUnknown; // No element of this type
    // Otherwise the file is being scanned: this function is called again once the scan is over
}

KeyboardDisplayContent DirectoryBrowser::getKeyboardDisplayContent()
{
    KeyboardDisplayContent content;

    // File selected
    QModelIndex index = ui->listView->currentIndex();
    int fileCount = ui->listView->model()->rowCount();
    if (index.isValid())
    {
        const DirectoryFileData * d = index.data(Qt::UserRole).value<const DirectoryFileData *>();
        if (d != nullptr)
            content.fileName = d->getFileName();
        content.hasPreviousFile = index.row() > 0;
        content.hasNextFile = index.row() < fileCount - 1;
    }
    else
        content.hasNextFile = fileCount > 0;

    // Element selected
    DirectoryElementListView * list = getSelectedList();
    if (list != nullptr)
    {
        int row = list->getSelectedRow();
        content.elementType = list->getType();
        list->getSelectedTexts(content.elementNumber, content.elementName);
        content.hasPreviousElement = row > 0;
        content.hasNextElement = row < list->getRowCount() - 1;
    }
    else
        content.hasNextElement = (getFirstList() != nullptr);

    return content;
}

void DirectoryBrowser::scheduleKeyboardDisplayUpdate()
{
    if (_dialogKeyboard == nullptr || _keyboardDisplayUpdateScheduled)
        return;
    _keyboardDisplayUpdateScheduled = true;
    QMetaObject::invokeMethod(this, [this]() {
        _keyboardDisplayUpdateScheduled = false;
        updateKeyboardDisplay();
    }, Qt::QueuedConnection);
}

void DirectoryBrowser::updateKeyboardDisplay()
{
    // This explorer may not be the one that is displayed
    if (_dialogKeyboard != nullptr && this->isVisible())
        _dialogKeyboard->setDisplayContent(getKeyboardDisplayContent());
}

void DirectoryBrowser::hideEvent(QHideEvent * event)
{
    Tab::hideEvent(event);
    _preview->onHidden();
}

void DirectoryBrowser::focusFileList()
{
    // Select the first element in the list (if any)
    if (ui->listView->model() && ui->listView->model()->rowCount() > 0)
    {
        ui->listView->setFocus();
        if (!ui->listView->selectionModel()->hasSelection())
            ui->listView->setCurrentIndex(ui->listView->model()->index(0, 0));
    }
}

bool DirectoryBrowser::eventFilter(QObject *obj, QEvent *event)
{
    // Navigation with arrows between widgets
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        switch (keyEvent->key())
        {
        case Qt::Key_Return: case Qt::Key_Enter:
            if (obj == ui->lineSearch)
            {
                focusFileList();
                return true;
            }
            break;
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
                focusFileList();
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