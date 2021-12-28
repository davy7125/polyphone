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

#include "soundfonteditorfiles.h"
#include "ui_soundfonteditorfiles.h"
#include "soundfontdownloaddata.h"
#include "soundfontfilecell.h"
#include "contextmanager.h"
#include <QFileDialog>
#include <QMessageBox>

const int SoundfontEditorFiles::MAX_FILES = 10;
const int SoundfontEditorFiles::MAX_MB = 100;

SoundfontEditorFiles::SoundfontEditorFiles(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SoundfontEditorFiles),
    _newFileIndex(-1)
{
    ui->setupUi(this);
}

SoundfontEditorFiles::~SoundfontEditorFiles()
{
    while (!_cells.isEmpty())
        delete _cells.take(_cells.keys()[0]);
    delete ui;
}

static bool sortFunction(const SoundfontDownloadData *s1, const SoundfontDownloadData *s2)
{
    return s1->getOrdering() < s2->getOrdering();
}

void SoundfontEditorFiles::initialize(QList<SoundfontDownloadData *> data)
{
    // First order the list
    std::sort(data.begin(), data.end(), sortFunction);

    // Clear and add each file information
    while (!_cells.isEmpty())
        delete _cells.take(_cells.keys()[0]);
    for (int i = 0; i < data.count(); i++)
    {
        SoundfontFileCell * cell = new SoundfontFileCell(this);
        connect(cell, SIGNAL(removed(int)), this, SLOT(deleteCell(int)));
        connect(cell, SIGNAL(replaced(int,QString)), this, SLOT(replaceCell(int,QString)));
        cell->initialize(data[i]);
        ui->verticalLayout->addWidget(cell);
        _cells[data[i]->getId()] = cell;
    }
}

void SoundfontEditorFiles::addFile()
{
    // 10 files max
    if (_cells.count() >= MAX_FILES)
    {
        QMessageBox::warning(this, tr("Warning"), tr("The number of files cannot exceed %1.").arg(MAX_FILES));
        return;
    }

    // Get a new file
    QString newFilePath = getFilePath(ContextManager::recentFile()->getLastDirectory(RecentFileManager::FILE_TYPE_UPLOAD));
    if (newFilePath.isEmpty())
        return;

    // Add a cell
    SoundfontFileCell * cell = new SoundfontFileCell(this);
    connect(cell, SIGNAL(removed(int)), this, SLOT(deleteCell(int)));
    connect(cell, SIGNAL(replaced(int,QString)), this, SLOT(replaceCell(int,QString)));
    cell->initialize(newFilePath, _newFileIndex);
    ui->verticalLayout->addWidget(cell);
    _cells[_newFileIndex] = cell;
    _newFileIndex--;
}

void SoundfontEditorFiles::replaceCell(int id, QString previousFilePath)
{
    // Check that the id is known
    if (!_cells.contains(id))
        return;

    // Get a new file
    if (previousFilePath.isEmpty())
        previousFilePath = ContextManager::recentFile()->getLastDirectory(RecentFileManager::FILE_TYPE_UPLOAD);
    QString newFilePath = getFilePath(previousFilePath);
    if (newFilePath.isEmpty() || newFilePath == previousFilePath)
        return;

    // Replace it in the cell
    _cells[id]->fileReplaced(newFilePath);
}

void SoundfontEditorFiles::deleteCell(int id)
{
    if (_cells.contains(id))
        delete _cells.take(id);
}

QString SoundfontEditorFiles::getFilePath(QString path)
{
    // Open an executable
    QString newFile = QFileDialog::getOpenFileName(this, tr("File to upload"), path, tr("Files") + " (*.sf2 *.sf3 *.doc *.odt *.pdf *.txt *.jpg *.png *.zip *.rar)");
    if (newFile.isEmpty())
        return "";
    QFileInfo info(newFile);
    if (!info.exists())
        return "";

    // Add the selected file in the recent files
    ContextManager::recentFile()->addRecentFile(RecentFileManager::FILE_TYPE_UPLOAD, newFile);

    // Check the size
    if (info.size() > MAX_MB * 1000000)
    {
        QMessageBox::warning(this, tr("Warning"), tr("The file size cannot exceed %1 MB.").arg(MAX_MB));
        return "";
    }

    return newFile;
}

void SoundfontEditorFiles::fillArguments(QMap<QString, QString> &arguments)
{
    int count = 0;
    arguments["file_number"] = QString::number(_cells.count());
    foreach (int id, _cells.keys())
    {
        QString prefix = "file_" + QString::number(count) + "_";
        arguments[prefix + "id"] = (id >= 0) ? QString::number(id) : "";
        arguments[prefix + "title"] = _cells[id]->getTitle();
        arguments[prefix + "description"] = _cells[id]->getDescription();
        arguments[prefix + "name"] = QFileInfo(_cells[id]->getFilePath()).fileName();
        count++;
    }
}

QMap<QString, QString> SoundfontEditorFiles::getFileArguments()
{
    QMap<QString, QString> map;
    int count = 0;
    foreach (int id, _cells.keys())
    {
        QString prefix = "file_" + QString::number(count) + "_";
        map[prefix + "data"] = _cells[id]->getFilePath();
        count++;
    }
    return map;
}
