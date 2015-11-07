/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2015 Davy Triponney                                **
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
**  Website/Contact: http://www.polyphone.fr/                             **
**             Date: 01.01.2013                                           **
***************************************************************************/

#ifndef DIALOG_EXPORT_H
#define DIALOG_EXPORT_H

#include <QDialog>
#include "sf2_types.h"

class Pile_sf2;

namespace Ui {
class DialogExport;
}

class DialogExport : public QDialog
{
    Q_OBJECT
    
public:
    DialogExport(Pile_sf2 * sf2, QList<EltID> listSf2, QWidget *parent = 0);
    ~DialogExport();
    
private slots:
    void on_pushTick_clicked();
    void on_pushUntick_clicked();
    void on_pushFolder_clicked();
    void on_pushAnnuler_clicked();
    void on_pushExport_clicked();
    void on_comboFormat_currentIndexChanged(int index);

signals:
    void accepted(QList<QList<EltID> > listID, QString dir, int format,
                  bool presetPrefix, bool bankDir, bool gmSort, int quality);

private:
    Ui::DialogExport *ui;
};

#endif // DIALOG_EXPORT_H
