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

DirectoryBrowser::DirectoryBrowser(QWidget *parent) : QWidget(parent),
    ui(new Ui::DirectoryBrowser),
    _watcher(nullptr)
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
    QString titleStyleSheet = "QLabel#labelFilters, QFrame#frameTop{background-color:" + highlightedBackground + ";color:" + highlightedText+ "}";
    ui->frameTop->setStyleSheet(titleStyleSheet);
    ui->lineSearch->setStyleSheet("QLineEdit{background-color:" + highlightedText + ";color:" + highlightedBackground + ";border:0;border-radius:2px;}");
    QColor color = ThemeManager::mix(
        ContextManager::theme()->getColor(ThemeManager::LIST_BACKGROUND),
        ContextManager::theme()->getColor(ThemeManager::LIST_TEXT), 0.5);
    ui->labelNoResults->setStyleSheet("QLabel{color:" + color.name() + ";border:1px solid " + border + ";border-top:0;border-right:0;border-bottom:0}");
    ui->tableView->hide();

    // Connections
    connect(ui->widgetDisplayMenu, SIGNAL(displayOptionsChanged(int)), ui->tableView, SLOT(setDisplayOptions(int)));
    connect(ui->widgetSortMenu, SIGNAL(currentIndexChanged(int)), ui->tableView, SLOT(setSortType(int)));
    connect(ui->lineSearch, SIGNAL(textEdited(QString)), ui->tableView, SLOT(setFilter(QString)));
    connect(ui->tableView, SIGNAL(contentChanged()), this, SLOT(onContentChanged()));
    connect(ui->tableView, SIGNAL(itemDoubleClicked(QString,EltID)), this, SIGNAL(itemClicked(QString,EltID)));

    // Initial sort type
    ui->tableView->setSortType(ui->widgetSortMenu->currentIndex());
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

    // Browse the files
    QMap<QString, QDateTime> newFiles;
    QFileInfoList files = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
    for (const QFileInfo &fileInfo : files)
    {
        newFiles[fileInfo.absoluteFilePath()] = fileInfo.lastModified();
        if (_currentFiles.contains(fileInfo.absoluteFilePath()))
        {
            if (_currentFiles[fileInfo.absoluteFilePath()] != newFiles[fileInfo.absoluteFilePath()])
                ui->tableView->updateFile(new DirectoryFileData(fileInfo));
            _currentFiles.remove(fileInfo.absoluteFilePath());
        }
        else
            ui->tableView->addFile(new DirectoryFileData(fileInfo));
    }
    foreach (QString absoluteFilePath, _currentFiles.keys())
        ui->tableView->removeFile(absoluteFilePath);
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
    if (ui->tableView->model()->rowCount() > 0)
    {
        ui->labelNoResults->hide();
        ui->tableView->show();
    }
    else
    {
        ui->tableView->hide();
        ui->labelNoResults->show();
    }
}