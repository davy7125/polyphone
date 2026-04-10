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

DirectoryBrowser::DirectoryBrowser(QWidget *parent) : QWidget(parent),
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

    // Connections
    //connect(ui->widgetDisplayMenu, SIGNAL(displayOptionsChanged(int)), ui->listView, SLOT(setDisplayOptions(int)));
    connect(ui->widgetSortMenu, SIGNAL(currentIndexChanged(int)), ui->listView, SLOT(setSortType(int)));
    connect(ui->lineSearch, SIGNAL(textEdited(QString)), ui->listView, SLOT(setFilter(QString)));
    connect(ui->listView, SIGNAL(contentChanged()), this, SLOT(onContentChanged()));
    connect(ui->listSamples, SIGNAL(itemDoubleClicked(QString,EltID)), this, SIGNAL(itemDoubleClicked(QString,EltID)));
    connect(ui->listInstruments, SIGNAL(itemDoubleClicked(QString,EltID)), this, SIGNAL(itemDoubleClicked(QString,EltID)));
    connect(ui->listPresets, SIGNAL(itemDoubleClicked(QString,EltID)), this, SIGNAL(itemDoubleClicked(QString,EltID)));
    connect(ui->listView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(onSelectionChanged(QItemSelection,QItemSelection)));
    connect(ui->listView, SIGNAL(renameRequested(QString)), this, SLOT(onRenameRequested(QString)));
    connect(ui->listView, SIGNAL(deleteRequested(QString)), this, SLOT(onDeleteRequested(QString)));

    // Splitter
    CustomSplitter * splitter = new CustomSplitter(this, ui->widgetLeft, ui->widgetRight, "directory_browser_splitter_sizes");
    QGridLayout * layout = dynamic_cast<QGridLayout *>(ui->pageSoundfonts->layout());
    layout->addWidget(splitter, 1, 0);
    splitter->setHandleWidth(6);

    // Initial sort type
    ui->listView->setSortType(ui->widgetSortMenu->currentIndex());
}

DirectoryBrowser::~DirectoryBrowser()
{
    delete ui;
}

void DirectoryBrowser::initialize(QString dirPath)
{
    _dirPath = dirPath;
    ui->spinner->startAnimation();

    // Directory checks
    QDir dir(dirPath);
    if (!dir.exists())
    {
        ui->labelError->setText(tr("The directory \"%1\" doesn't exist.").arg(dirPath));
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

void DirectoryBrowser::on_listView_doubleClicked(const QModelIndex &index)
{
    const DirectoryFileData * d = index.data(Qt::UserRole).value<const DirectoryFileData *>();
    if (d == nullptr ||
        d->getStatus() == DirectoryFileData::NOT_READABLE ||
        d->getStatus() == DirectoryFileData::NOT_OPENABLE)
        return;

    emit itemDoubleClicked(d->getPath(), EltID(elementSf2));
}

void DirectoryBrowser::onRenameRequested(QString path)
{
    QFileInfo info(path);
    QString currentName = info.fileName();

    bool ok = false;
    QString newName = QInputDialog::getText(this, tr("Rename"), tr("New name:"), QLineEdit::Normal, currentName, &ok);
    if (!ok || newName.isEmpty() || newName == currentName)
        return;

    QDir dir = info.dir();
    QString newPath = dir.filePath(newName);
    if (QFile::exists(newPath))
    {
        QMessageBox::warning(this, tr("Error"), tr("A file with this name already exists."));
        return;
    }

    if (!QFile::rename(path, newPath))
        QMessageBox::warning(this, tr("Error"), tr("Cannot rename file \"%1\".").arg(currentName));
}

void DirectoryBrowser::onDeleteRequested(QString path)
{
    QFileInfo info(path);
    if (QMessageBox::question(this, tr("Confirm deletion"),
        tr("Are you sure you want to delete file \"%1\"?").arg(info.fileName()), QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes)
        return;

    if (!QFile::remove(path))
        QMessageBox::warning(this, tr("Error"), tr("Cannot delete file \"%1\".").arg(info.fileName()));
}
