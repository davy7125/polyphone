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

#ifndef CONFIGTOC_H
#define CONFIGTOC_H

#include <QWidget>

namespace Ui {
class ConfigToc;
}

class ConfigToc : public QWidget
{
    Q_OBJECT

public:
    explicit ConfigToc(QWidget *parent = nullptr);
    ~ConfigToc();

signals:
    void sectionClicked(int sectionNumber);

private slots:
    void on_buttonGeneral_clicked();
    void on_buttonInterface_clicked();
    void on_buttonSound_clicked();
    void on_buttonKeyboard_clicked();
    void on_buttonRepository_clicked();

private:
    Ui::ConfigToc *ui;
};

#endif // CONFIGTOC_H
