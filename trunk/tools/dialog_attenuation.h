/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2016 Davy Triponney                                **
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

#ifndef DIALOG_ATTENUATION_H
#define DIALOG_ATTENUATION_H

#include <QDialog>

namespace Ui {
class DialogAttenuation;
}

class DialogAttenuation : public QDialog
{
    Q_OBJECT

public:
    DialogAttenuation(double minInst, double maxInst, double minPrst, double maxPrst, QWidget *parent = 0);
    ~DialogAttenuation();

signals:
    void accepted(double offsetInst, double offsetPrst);

private slots:
    void accept();

private:
    Ui::DialogAttenuation *ui;
    static QString formatDouble(double value);
};

#endif // DIALOG_ATTENUATION_H
