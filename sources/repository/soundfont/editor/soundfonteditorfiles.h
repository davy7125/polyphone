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

#ifndef SOUNDFONTEDITORFILES_H
#define SOUNDFONTEDITORFILES_H

#include <QWidget>
#include <QList>
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

private:
    Ui::SoundfontEditorFiles *ui;
    QList<SoundfontFileCell*> _cells;
};

#endif // SOUNDFONTEDITORFILES_H
