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

#ifndef SOUNDFONTDOWNLOADCELL_H
#define SOUNDFONTDOWNLOADCELL_H

#include <QWidget>
class SoundfontDownloadData;

namespace Ui {
class SoundfontDownloadCell;
}

class SoundfontDownloadCell : public QWidget
{
    Q_OBJECT

public:
    explicit SoundfontDownloadCell(QWidget *parent = nullptr);
    ~SoundfontDownloadCell();

    void initialize(SoundfontDownloadData * data);

private slots:
    void on_pushDownload_clicked();

private:
    Ui::SoundfontDownloadCell *ui;
    int _id;
    QString _name;
};

#endif // SOUNDFONTDOWNLOADCELL_H
