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

#ifndef SOUNDFONTEDITORFILES_H
#define SOUNDFONTEDITORFILES_H

#include <QWidget>
#include <QMap>
class SoundfontDownloadData;
class SoundfontFileCell;

namespace Ui {
class SoundfontEditorFiles;
}

class SoundfontEditorFiles : public QWidget
{
    Q_OBJECT

public:
    explicit SoundfontEditorFiles(QWidget *parent = nullptr);
    ~SoundfontEditorFiles();

    // Initialize the file list
    void initialize(QList<SoundfontDownloadData *> data);

    // Click on "add a file"
    void addFile();

    // Number of files
    int fileCount() { return _cells.count(); }

    // Get a description of the editing
    void fillArguments(QMap<QString, QString> &arguments);

    // Get the file list to upload
    QMap<QString, QString> getFileArguments();

private slots:
    void replaceCell(int id, QString previousFilePath);
    void deleteCell(int id);

private:
    QString getFilePath(QString path);

    Ui::SoundfontEditorFiles *ui;
    QMap<int, SoundfontFileCell*> _cells;
    int _newFileIndex;

    static const int MAX_FILES;
    static const int MAX_MB;
};

#endif // SOUNDFONTEDITORFILES_H
