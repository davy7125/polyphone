/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2017 Davy Triponney                                **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: http://polyphone-soundfonts.com                      **
**             Date: 01.01.2013                                           **
***************************************************************************/

#ifndef DIALOG_EXPORTLIST_H
#define DIALOG_EXPORTLIST_H

#include <QDialog>
#include "basetypes.h"
#include "soundfontmanager.h"
#include <QMap>

namespace Ui {
class DialogExportList;
}

class DialogExportList : public QDialog
{
    Q_OBJECT

public:
    explicit DialogExportList(SoundfontManager * sf2, EltID id, QWidget *parent = 0);
    ~DialogExportList();

private slots:
    void on_radioCsv_clicked();
    void on_radioHtml_clicked();
    void on_pushCopy_clicked();

private:
    bool _init;
    Ui::DialogExportList *ui;
    QMap<int, QMap<int, QString> > _mapName;
};

#endif // DIALOG_EXPORTLIST_H
