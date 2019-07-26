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

#ifndef SOUNDFONTFILECELL_H
#define SOUNDFONTFILECELL_H

#include <QWidget>
class SoundfontDownloadData;

namespace Ui {
class SoundfontFileCell;
}

class SoundfontFileCell : public QWidget
{
    Q_OBJECT

public:
    explicit SoundfontFileCell(QWidget *parent = nullptr);
    ~SoundfontFileCell();

    // Initialize the cell
    void initialize(SoundfontDownloadData * data);
    void initialize(QString filePath);

private:
    Ui::SoundfontFileCell *ui;

    int _id; // id from the server, -1 for new files
    QString _filePath;
};

#endif // SOUNDFONTFILECELL_H
